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
    while(micros()<lastSample + (1E6)/samplingFrequency) {}
    //For the moment, this is analog over pin 36
    lastSample = micros();
    insertValue(analogRead(36));
}

uint16_t fakeSample(float frequency, int i) {
    // i is the step in the loop at this point
    float step = i * samplingFrequency;
    auto sine = sin(i*2*PI*frequency/samplingFrequency);
    return (sine + 1)*32767; // normalise to 0-65535
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
    //init_send();
    pinMode(36, INPUT); // Set up microphone pin
    pinMode(38, INPUT); // Set up dBA meter
    dbg_print("Initialised!");
}

void loop(){
    int lastMillis = 0;
    uint16_t samplingPeriod = round(1000000 * (1.0 / samplingFrequency));
    while(true) {
        //send_loop();
        // Sample audio
        for (int i=0;i<FFT_SAMPLES;i++){
            lastMillis = micros();
            //insertValue(analogRead(36), i);
            insertValue(fakeSample(440,i),i);
            while ((micros() - lastMillis) < samplingPeriod) { /* do nothing to wait */ }
        }
        uint16_t bands[8];
        memset(&bands, 0, sizeof(bands));
        const valuePack data = generateFFT(bands, boundaries);
        const double dba = sampleDBA();

        //sendData(data, dba);
        //DEBUG: Blocking loop
        sleep(UPDATE_INTERVAL);
    }
}