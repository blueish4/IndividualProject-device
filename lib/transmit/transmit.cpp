/**
 * transmit.cpp
 * Library file to contain the transmission methods and helpers
 * Copyright (C) blueish 2020
 */

#include "config.h"

#include <WiFiClientSecure.h>
#include <esp_wpa2.h>
#include "esp32-mqtt.h"
#include "debug.h"
#include "pack.h"

void send_loop() {
    mqtt->loop();
    delay(10);  // <- fixes some issues with WiFi stability

    if (!mqttClient->connected()) {
        connect();
    }
}

bool sendData(const char* data, int length) {
    #ifdef WIFI_ENABLED
    publishTelemetry(data, length);
    #endif
    return true;
}

void sendData(valuePack p, double dba) {
    void* sendBuffer = malloc(sizeof(double)*2+sizeof(uint16_t)*8);
    memcpy(sendBuffer, &p.majorPeak.frequency, sizeof(p.majorPeak.frequency));
    memcpy(sendBuffer+sizeof(double), &dba, sizeof(dba));
    const int offset = (int)sendBuffer+sizeof(double)*2;
    for(int i=0;i<8;i++) {  //TODO: prune magic number here
        memcpy((void*)offset+i*sizeof(uint16_t), &p.frequencies[i], sizeof(uint16_t));
        p.frequencies[i] = 0;
    }
    sendData((const char*)sendBuffer, sizeof(double)*2+sizeof(uint16_t)*8);
    free(sendBuffer);
}

void init_send() {
    #if WIFI_ENABLED
    dbg_print("wifi support on");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    #ifdef EAP_IDENTITY
    //WPA2-Enterprise (eduroam)
    esp_wifi_sta_wpa2_ent_set_username((uint8_t *)EAP_IDENTITY, strlen(EAP_IDENTITY));
    esp_wifi_sta_wpa2_ent_set_password((uint8_t *)EAP_PASSWORD, strlen(EAP_PASSWORD));
    esp_wpa2_config_t config = WPA2_CONFIG_INIT_DEFAULT(); //set config settings to default
    esp_wifi_sta_wpa2_ent_enable(&config); //set config settings to enable function
    WiFi.begin(WIFI_SSID); //connect to wifi
    #else
    // WPA2-PSK
    WiFi.begin(WIFI_SSID, WIFI_PSK);
    #endif

    int counter = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        dbg_print("connecting");
        counter++;
        if(counter>=60){ //after 30 seconds timeout - reset board
            ESP.restart();
        }
    }
    dbg_print("connected!");

    configTime(0, 0, ntp_primary, ntp_secondary);
    dbg_print("syncing time");
    while (time(nullptr) < 1510644967) {
        delay(10);
    }

    device = new CloudIoTCoreDevice(
      project_id, location, registry_id, device_id,
      private_key_str);

    netClient = new WiFiClientSecure();
    mqttClient = new MQTTClient(512);
    mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
    mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);
    mqtt->setUseLts(false);
    mqtt->startMQTT();
    #endif
}