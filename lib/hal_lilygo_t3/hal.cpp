#include "hal.h"
#include "lvgl.h"


#include <Arduino.h>
#include "display.h"
#include "WiFi.h"
#include "cardputer_conf.h"

void hal_setup(void) {
    Display::init();

    WiFiClass::mode(WIFI_MODE_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFiClass::status() != WL_CONNECTED) {
        delay(500);
        ESP_LOGD("main", "Connecting to WiFi..");
    }
    ESP_LOGD("main", "connected to wifi");
    ESP_LOGD("main", "IP Address: %s", WiFi.localIP().toString().c_str());
    ESP_LOGD("main", "MAC Address: %s", WiFi.macAddress().c_str());
    ESP_LOGD("main", "Gateway: %s", WiFi.gatewayIP().toString().c_str());
    ESP_LOGD("main", "Subnet Mask: %s", WiFi.subnetMask().toString().c_str());
    ESP_LOGD("main", "DNS: %s", WiFi.dnsIP().toString().c_str());
}

[[noreturn]] void hal_loop(void) {
    while (true) {
        lv_timer_handler();
        delay(10);
    }
}