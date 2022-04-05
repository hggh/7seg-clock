#include <Arduino.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <ESPmDNS.h>
#include <time.h>

#include <FS.h>
#include <FastLED.h>
#include <Bounce2.h>
#include <WiFiManager.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#include "numbers.h"

#define LED_COUNT 58
#define PIN_RGB_LEDS 17
#define PIN_WIFI_RESET 34
#define HOSTNAME "clocki-7seg"
#define uS_TO_S_FACTOR 1000000ULL

CRGB leds[LED_COUNT];
CRGB led_color = CRGB::Blue;
String html_color_code = "#0039e6";
bool update_leds = false;

WebServer server(80);
WiFiManager wm;
Bounce2::Button wifi_cfg_reset = Bounce2::Button();

const long gmt_offset_sec = -3600;
const int day_light_offset_sec = 3600;

uint8_t time_hour = 255;
uint8_t time_minute = 255;

struct Config{
  uint8_t led_brightness;
  char color[10];
  bool time_sleep_enable;
  uint8_t time_sleep_hour;
  uint8_t time_sleep_duration;
};
Config config;

void fill_color_from_config(Config &config) {
  String ctmp = String(config.color);
  ctmp.replace("#", "");
  long color_code = strtol(ctmp.c_str(), NULL, 16);
  led_color = CRGB(color_code);
}

void load_configuration(Config &config) {
  File file = SPIFFS.open("/esp32-config.txt");
  StaticJsonDocument<300> doc;
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read file, using default configuration"));
  }
  config.led_brightness = doc["led_brightness"] | 70;
  strlcpy(config.color, doc["color"] | "#89CFF0", sizeof(config.color));
  config.time_sleep_enable = doc["time_sleep_enable"] | false;
  config.time_sleep_hour = doc["time_sleep_hour"] | 22;
  config.time_sleep_duration = doc["time_sleep_duration"] | 8;

  fill_color_from_config(config);

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close();
}

void save_configuration(Config &config) {
  SPIFFS.remove("/esp32-config.txt");
  File file = SPIFFS.open("/esp32-config.txt", FILE_WRITE);

  StaticJsonDocument<300> doc;
  doc["led_brightness"] = config.led_brightness;
  doc["color"] = config.color;

  doc["time_sleep_enable"] = config.time_sleep_enable;
  doc["time_sleep_hour"] = config.time_sleep_hour;
  doc["time_sleep_duration"] = config.time_sleep_duration;

  fill_color_from_config(config);

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }
  file.close();
}

void write_number_to_leds(uint8_t offset, uint8_t number) {
  for (uint8_t i = 0; i < 14; i++) {
    if (numbers[number][i] == 1) {
      leds[offset + i] = led_color;
    }
  }
}

void clock_display_number(uint8_t offset, uint8_t number, boolean display_zero = false) {
  uint8_t t_first = ((number / 10 ) % 10);
  uint8_t t_second = (number % 10 );

  if (display_zero == true || t_first != 0 ) {
    write_number_to_leds(offset, t_first);
  }
  write_number_to_leds(offset + 14, t_second);
}


void webHandleRoot() {
  String content = "";
  File file = SPIFFS.open("/index.html");
  while(file.available()){
    content += char(file.read());
  }
  content.replace("LED_BRIGHTNESS_VALUE", String(config.led_brightness));
  content.replace("LED_SINGLE_COLOR_CODE", String(config.color));
  if (config.time_sleep_enable == true) {
    content.replace("TIME_SLEEP_ENABLE", String("enable"));
  }
  else {
    content.replace("TIME_SLEEP_ENABLE", String("disable"));
  }
  content.replace("TIME_SLEEP_HOUR", String(config.time_sleep_hour));
  content.replace("TIME_SLEEP_DURATION", String(config.time_sleep_duration));

  file.close();
  server.send(200, "text/html", content);
}

void webHandleUpdate() {
  String req_color_single = server.arg("color");
  req_color_single.toCharArray(config.color, 10);

  String req_brightness = server.arg("brightness");
  config.led_brightness = req_brightness.toInt();

  String req_time_sleep_enable = server.arg("time_sleep_enable");
  if (req_time_sleep_enable.equals("enable")) {
    config.time_sleep_enable = true;
  }
  else {
    config.time_sleep_enable = false;
  }

  String req_time_sleep_hour = server.arg("time_sleep_hour");
  config.time_sleep_hour =  req_time_sleep_hour.toInt();

  String req_time_sleep_duration = server.arg("time_sleep_duration");
  config.time_sleep_duration =  req_time_sleep_duration.toInt();

  FastLED.setBrightness(config.led_brightness);
  save_configuration(config);

  update_leds = true;
  webHandleRoot();
}


void wm_config_mode_callback(WiFiManager *myWiFiManager) {
  Serial.println("Entering WiFI Manager configuration....");

  fill_solid(leds, LED_COUNT, CRGB::DeepPink);
  FastLED.show();
}

void goto_sleep(unsigned int hours = 1) {
  Serial.println("Goto sleep...");
  FastLED.clear(true);
  esp_wifi_stop();
  delay(10);
  esp_sleep_enable_timer_wakeup((uint64_t)(hours * 60 * 60) * (uint64_t)uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}

void setup() {
  btStop();
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, PIN_RGB_LEDS>(leds, LED_COUNT);

  if(!SPIFFS.begin(true)){
    Serial.println("error on SPIFFS...");
  }
  std::vector<const char *> wm_menu  = {"wifi", "exit"};
  wm.setConnectTimeout(30);
  wm.setAPCallback(wm_config_mode_callback);
  wm.setShowInfoUpdate(true);
  wm.setShowInfoErase(false);
  wm.setMenu(wm_menu);
  wm.autoConnect(HOSTNAME, "");

  load_configuration(config);

  FastLED.setBrightness(config.led_brightness);
  fill_solid(leds, LED_COUNT, CRGB::Black);
  FastLED.show();

  server.on("/", webHandleRoot);
  server.on("/update", webHandleUpdate);
  server.begin();

  MDNS.begin(HOSTNAME);
  MDNS.addService("http", "tcp", 80);
  configTime(gmt_offset_sec, day_light_offset_sec, "0.de.pool.ntp.org", "1.de.pool.ntp.org", "2.de.pool.ntp.org");
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();

  wifi_cfg_reset.attach(PIN_WIFI_RESET, INPUT);
  wifi_cfg_reset.interval(25);
  wifi_cfg_reset.setPressedState(LOW);
}

void loop() {
  wifi_cfg_reset.update();
  server.handleClient();

  if (wifi_cfg_reset.pressed()) {
    // WiFi reset configuration button was pressed
    wm.resetSettings();
    Serial.println("WiFi reset configuration button was pressed......");
    Serial.flush();
    delay(500);
    ESP.restart();
  }

  if (WiFi.status() != WL_CONNECTED) {
    wm.autoConnect(HOSTNAME, "");
  }

  struct tm timedate;
  getLocalTime(&timedate);

  if (timedate.tm_min != time_minute || timedate.tm_hour != time_hour || update_leds == true) {
    update_leds = false;
    // time has changed, display it on the WS2812b LEDs
    time_minute = timedate.tm_min;
    time_hour = timedate.tm_hour;
    fill_solid(leds, LED_COUNT, CRGB::Black);

    if (time_hour == 0) {
      clock_display_number(0, time_hour, true);
    }
    else {
      clock_display_number(0, time_hour, false);
    }
    clock_display_number(30, time_minute, true);

    // dots
    leds[28] = led_color;
    leds[29] = led_color;

    FastLED.show();
  }
  if (config.time_sleep_enable == true && config.time_sleep_hour == timedate.tm_hour && timedate.tm_min == 0) {
    goto_sleep(config.time_sleep_duration);
  }
  delay(1);
}
