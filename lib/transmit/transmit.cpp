/**
 * transmit.cpp
 * Library file to contain the transmission methods and helpers
 * Copyright (C) Michael Hathaway 2020
 */
#include <config.h>
#include <lmic.h>
#include <hal/hal.h>
#include "debug.h"
#include "SPI.h"
#include <Arduino.h>

#ifdef LORA_ENABLED
static u1_t PROGMEM NWKSKEY[16] = LORA_NETWORK_KEY;
static u1_t PROGMEM APPSKEY[16] = LORA_APP_KEY;

static  u4_t DEVADDR = LORA_DEV_ADDR;

// These callbacks are only used in over-the-air activation, so they are
// left empty here (we cannot leave them out completely unless
// DISABLE_JOIN is set in config.h, otherwise the linker will complain).
void os_getArtEui (u1_t* buf) { }
void os_getDevEui (u1_t* buf) { }
void os_getDevKey (u1_t* buf) { }

// Pin mapping
const lmic_pinmap lmic_pins = {
    .nss = 18,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 14,
    .dio = {26, 33, 32},
};

#endif

bool sendData(int* data) {
    #ifdef LORA_ENABLED
    
    #endif

    #ifdef WIFI_ENABLED

    #endif

    #if !(WIFI_ENABLED || LORA_ENABLED)

    #endif
}

void init_lora(){
    SPI.begin(5, 19, 27);
    // LMIC init
    os_init_ex(&lmic_pins);
    // Reset the MAC state. Session and pending data transfers will be discarded.
    LMIC_reset();
    LMIC_setSession (0x1, DEVADDR, NWKSKEY, APPSKEY);
    LMIC_setupChannel(0, 868100000, DR_SF7,  BAND_CENTI);      // g-band

    #ifdef LORA_SINGLE_CHANNEL
    for(int i=1;i<9;i++){
        LMIC_disableChannel(i);
    }
    #else
    LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      // g-band
    LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      // g-band
    LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);      // g2-band
    #endif

    // Disable link check validation
    LMIC_setLinkCheckMode(0);

    // Set data rate and transmit power for uplink (note: txpow seems to be ignored by the library)
    LMIC_setDrTxpow(DR_SF7,14);
}

enum lora_status {
    timeout = EV_SCAN_TIMEOUT,

};
