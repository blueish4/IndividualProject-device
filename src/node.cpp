/**
 * Main program for ESP32 to monitor noise type and loudness
 * Copyright (C) blueish 2020
 */
#include <Arduino.h>
#include <string>
#include <sstream>

#include "transmit.h"
#include "debug.h"
#include "transform.h"
#include "pack.h"
#include "cfg.h"

double boundaries[9] = {31.5, 63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0};
unsigned long lastSample = 0;
// retrieves an audio sample and puts it in the queue
// can be moved to an interrupt if required
void sampleAudio(){
    // block until we have waited long enough
    // this may undersample, but can't oversample
    while(micros()<lastSample + (1E6)/40000) {}
    //For the moment, this is analog over pin 36
    lastSample = micros();
    insertValue(analogRead(36));
}

void setup(){
    init_dbg();
    init_send();
    init_transform(boundaries);
    pinMode(36, INPUT);
    dbg_print("Initialised!");
}

void loop(){
    int lastMillis = 0;
    while(true) {
        //sampleAudio();
        //Serial.println(vReal[currentSample]);
        for (int i=0;i<1024;i++){
            sampleAudio();
        }
        uint16_t bands[9];
        const valuePack data = generateFFT(bands, boundaries);
        send_loop();

        // publish a message roughly every second (+ sample and calculate time).
        // this will end up junking a lot of data if sample time << 1 second
        if (millis() - lastMillis > UPDATE_INTERVAL*100) {
            lastMillis = millis();
            void* sendBuffer = malloc(sizeof(double)+sizeof(uint16_t)*9);
            memcpy(sendBuffer, &data.majorPeak, sizeof(data.majorPeak));
            for(int i=0;i<9;i++) {  //TODO: prune magic number here
                memcpy(sendBuffer+sizeof(double)+i*sizeof(uint16_t), &data.frequencies[i], sizeof(uint16_t));
                data.frequencies[i] = 0;
            }

            sendData((const char*)sendBuffer, sizeof(double)+sizeof(uint16_t)*9);
            free(sendBuffer);
        }
    }
}