#include <Arduino.h>
#include "SPIFFS.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <time.h>

#include <FastLED.h>
#include <Bounce2.h>
#include <FS.h>
#include <WiFiManager.h>
#include <WebServer.h>

#include "numbers.h"

#define LED_COUNT 58
#define AMBILIGHT_LED_COUNT 9
#define PIN_RGB_LEDS 17
#define PIN_RGB_AMBILIGHT_LEDS 16
#define PIN_WIFI_RESET 34
#define HOSTNAME "clocki"

CRGB leds[LED_COUNT];
CRGB ambilight_leds[AMBILIGHT_LED_COUNT];
CRGB led_color = CRGB::Blue;
int led_brightness = 60;
int ambilight_led_brightness = 30;
String html_color_code = "#0039e6";
bool update_leds = false;

WebServer server(80);
WiFiManager wm;
Bounce2::Button wifi_cfg_reset = Bounce2::Button();

const long gmt_offset_sec = -3600;
const int day_light_offset_sec = 3600;

uint8_t time_hour = 255;
uint8_t time_minute = 255;

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
  content.replace("LED_BRIGHTNESS_VALUE", String(led_brightness));
  content.replace("LED_AMBILIGHT_BRIGHTNESS_VALUE", String(ambilight_led_brightness));
  content.replace("LED_SINGLE_COLOR_CODE", html_color_code);
  file.close();
  server.send(200, "text/html", content);
}

void webHandleUpdate() {
  String req_color_single = server.arg("color");
  html_color_code = req_color_single;
  req_color_single.replace("#", "");

  long color_code = strtol(req_color_single.c_str(), NULL, 16);
  led_color = CRGB(color_code);

  String req_brightness = server.arg("brightness");
  led_brightness = req_brightness.toInt();

  String req_ambilight_brightness = server.arg("ambilight_brightness");
  ambilight_led_brightness = req_ambilight_brightness.toInt();

  update_leds = true;
  webHandleRoot();
}

void wm_config_mode_callback(WiFiManager *myWiFiManager) {
  Serial.println("Entering WiFI Manager configuration....");

  fill_solid(leds, LED_COUNT, CRGB::DeepPink);
  FastLED[0].showLeds(40);
}

void setup() {
  btStop();
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  WiFi.setSleep(WIFI_PS_NONE);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, PIN_RGB_LEDS>(leds, LED_COUNT);
  FastLED.addLeds<NEOPIXEL, PIN_RGB_AMBILIGHT_LEDS>(ambilight_leds, AMBILIGHT_LED_COUNT);

  if(!SPIFFS.begin(true)){
    Serial.println("error on SPIFFS...");
  }
  wm.setAPCallback(wm_config_mode_callback);
  wm.autoConnect(HOSTNAME, "");

  fill_solid(leds, LED_COUNT, CRGB::Black);
  FastLED[0].showLeds(led_brightness);
  fill_solid(ambilight_leds, AMBILIGHT_LED_COUNT, CRGB::Black);
  FastLED[1].showLeds(ambilight_led_brightness);

  server.on("/", webHandleRoot);
  server.on("/update", webHandleUpdate);
  server.begin();

  MDNS.begin(HOSTNAME);
  MDNS.addService("http", "tcp", 80);
  configTime(gmt_offset_sec, day_light_offset_sec, "0.de.pool.ntp.org", "1.de.pool.ntp.org", "2.de.pool.ntp.org");
  setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
  tzset();

  wifi_cfg_reset.attach(PIN_WIFI_RESET, INPUT_PULLUP);
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

    // ambilight
    fill_solid(ambilight_leds, AMBILIGHT_LED_COUNT, led_color);

    FastLED[0].showLeds(led_brightness);
    FastLED[1].showLeds(ambilight_led_brightness);
  }
  delay(1);
}
