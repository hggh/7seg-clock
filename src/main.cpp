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
#define PIN_RGB_LEDS 22
#define PIN_WIFI_RESET 17
#define HOSTNAME "clocki"

CRGB leds[LED_COUNT];
CRGB::HTMLColorCode led_color = CRGB::Red;
WebServer server(80);
WiFiManager wm;
Bounce2::Button wifi_cfg_reset = Bounce2::Button();

const long  gmt_offset_sec = -3600;
const int   day_light_offset_sec = 3600;

void write_number_to_leds(uint8_t offset, uint8_t number) {
  for (uint8_t i = 0; i < 14; i++) {
    if (numbers[number][i] == 1) {
      leds[offset + i] = led_color;
    }
  }
}

void webHandleRoot() {
  String content = "";
  File file = SPIFFS.open("/index.html");
  while(file.available()){
    content += char(file.read());
  }
  file.close();
  server.send(200, "text/html", content);
}

void wm_config_mode_callback(WiFiManager *myWiFiManager) {
  Serial.println("Entering WiFI Manager configuration....");

  fill_solid(leds, LED_COUNT, CRGB::DeepPink);
  FastLED.setBrightness(40);
  FastLED.show();
}

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.setHostname(HOSTNAME);
  Serial.begin(115200);

  FastLED.addLeds<NEOPIXEL, PIN_RGB_LEDS>(leds, LED_COUNT);
  FastLED.setBrightness(5);

  if(!SPIFFS.begin(true)){
    Serial.println("error on SPIFFS...");
  }
  wm.setAPCallback(wm_config_mode_callback);
  wm.autoConnect(HOSTNAME, "");

  FastLED.clear(true);
  FastLED.show();

  server.on("/", webHandleRoot);
  server.begin();

  MDNS.begin(HOSTNAME);
  MDNS.addService("http", "tcp", 80);
  configTime(gmt_offset_sec, day_light_offset_sec, "0.de.pool.ntp.org", "1.de.pool.ntp.org", "2.de.pool.ntp.org");

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
}
