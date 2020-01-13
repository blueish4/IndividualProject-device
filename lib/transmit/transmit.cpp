/**
 * transmit.cpp
 * Library file to contain the transmission methods and helpers
 * Copyright (C) blueish 2020
 */
#include <WiFiClientSecure.h>
#include "esp_wpa2.h"
#include "config.h"
#include "debug.h"
bool sendData(int* data) {
    #ifdef WIFI_ENABLED

    #endif

}

void init_send() {
    #ifdef WIFI_ENABLED
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
    esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
    WiFi.begin(WIFI_SSID); //connect to wifi
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        dbg_print("connecting");
        counter++;
        if(counter>=60){ //after 30 seconds timeout - reset board
            ESP.restart();
        }
    }
    #endif
    dbg_print("send ready");
}