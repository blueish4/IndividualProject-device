/**
 * Main program for ESP32 to monitor noise type and loudness
 * Copyright (C) blueish 2020
 */
#include <Arduino.h>
#include <string>

#include "transmit.h"
#include "debug.h"
#include "transform.h"
#include "pack.h"
#include "cfg.h"

double boundaries[8] = {63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0};
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

double sampleDBA() {
    double walking_average = 0;
    for (int i=1;i<=DECIBEL_SAMPLE_WIDTH;i++) {
        walking_average = (walking_average*(i-1)+(analogRead(38)/FFT_SAMPLES*50))/i;
    }
    return walking_average;
}

void setup(){
    init_dbg();
    init_transform(boundaries);
    init_send();
    pinMode(36, INPUT); // Set up microphone pin
    pinMode(38, INPUT); // Set up dBA meter
    dbg_print("Initialised!");
}

void loop(){
    int lastMillis = 0;
    while(true) {
        //sampleAudio();
        //Serial.println(vReal[currentSample]);
        for (int i=0;i<FFT_SAMPLES;i++){
            sampleAudio();
        }
        uint16_t bands[8];
        const valuePack data = generateFFT(bands, boundaries);
        const double dba = sampleDBA();
        send_loop();

        // publish a message roughly every second (+ sample and calculate time).
        // this will end up junking a lot of data if sample time << 1 second
        //if (millis() - lastMillis > UPDATE_INTERVAL*100) {
            lastMillis = millis();
            void* sendBuffer = malloc(sizeof(double)*2+sizeof(uint16_t)*8);
            memcpy(sendBuffer, &data.majorPeak, sizeof(data.majorPeak));
            memcpy(sendBuffer+sizeof(double), &dba, sizeof(dba));
            const int offset = (int)sendBuffer+sizeof(double)*2;
            for(int i=0;i<8;i++) {  //TODO: prune magic number here
                memcpy((void*)offset+i*sizeof(uint16_t), &data.frequencies[i], sizeof(uint16_t));
                data.frequencies[i] = 0;
            }

            sendData((const char*)sendBuffer, sizeof(double)*2+sizeof(uint16_t)*8);
            free(sendBuffer);
        //}
    }
}