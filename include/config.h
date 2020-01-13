/**
 * config.h
 * Allows features to be enabled and disabled, as well as specifying built-in config options
 * Copyright (C) 2020 Michael Hathaway
 */

#define WIFI_ENABLED 1
#define LORA_ENABLED 1

#ifndef DEBUG
#define DEBUG 1
#endif

// Transmit every n seconds
#define UPDATE_INTERVAL 60

#define WIFI_SSID "ssid"
#define WIFI_PSK "password"
#define MQTT_SERVER
#define MQTT_USERNAME
#define MQTT_PASSWORD

// Used in LORA ABP activation
#define LORA_NETWORK_KEY { 0x9D, 0x63, 0x13, 0x9E, 0x46, 0x50, 0xEF, 0x43, 0x58, 0x0D, 0x8D, 0x8A, 0xB0, 0x2E, 0x30, 0x24 }
#define LORA_APP_KEY { 0xA1, 0x72, 0xD1, 0x47, 0x6B, 0xB0, 0x5A, 0xD5, 0x77, 0xF7, 0x07, 0x8E, 0x75, 0x3F, 0xAA, 0xCB }
#define LORA_DEV_ADDR 0x2601125D

// Enable only one channel (868.1MHz), or all channels
#define LORA_SINGLE_CHANNEL 0