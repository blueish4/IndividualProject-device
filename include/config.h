/**
 * config.h
 * Allows features to be enabled and disabled, as well as specifying built-in config options
 * Copyright (C) 2020 blueish
 */

#define WIFI_ENABLED 1
#define LORA_ENABLED 1

#ifndef DEBUG
#define DEBUG 1
#endif

// Transmit every n seconds
#define UPDATE_INTERVAL 60

#define WIFI_SSID "ssid"
// Use this for WPA2-PSK
#define WIFI_PSK "password"
// Use this for WPA2-Enterprise (ie. eduroam)
#define EAP_IDENTITY "example@gla.ac.uk"
#define EAP_PASSWORD "password"


#define MQTT_SERVER
#define MQTT_USERNAME
#define MQTT_PASSWORD

// Used in LORA ABP activation
#define LORA_NETWORK_KEY
#define LORA_APP_KEY
#define LORA_DEV_ADDR

// Enable only one channel (868.1MHz), or all channels
#define LORA_SINGLE_CHANNEL 0