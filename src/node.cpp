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

#define FFT_AVERAGING_WINDOW 5

double boundaries[8] = {63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0};
unsigned long lastSample = 0;
// retrieves an audio sample and puts it in the queue
// can be moved to an interrupt if required
void sampleAudio(int iteration, int samplingPeriod){

    lastSample = micros();
    // Reading ADC while WiFi is on causes some intermittent issues
    // not sure how to do that without killing the connection though 😞
    insertValue(analogRead(36), iteration);
    //insertValue(fakeSample(50,i),i);
    while ((micros() - lastSample) < samplingPeriod) { /* do nothing to wait */ }
}

uint16_t fakeSample(float frequency, int i) {
    // i is the step in the loop at this point
    auto sine = sin(i*2*PI*frequency/samplingFrequency);
    return (sine + 1)*32767; // normalise to 0-65535
}

double sampleDBA() {
    double walking_average = 0;
    for (int i=1;i<=DECIBEL_SAMPLE_WIDTH;i++) {
        const float readValue = analogRead(38)/1024.0;
        walking_average = (walking_average*(i-1)+(readValue*50))/i;
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
    uint16_t samplingPeriod = round(1000000 * (1.0 / samplingFrequency));
    while(true) {
        send_loop();
        uint16_t fft_averages[FFT_SAMPLES/2];
        memset(fft_averages, 0, sizeof(fft_averages));
        for (int n=1;n<FFT_AVERAGING_WINDOW+1; n++){
            // Sample audio
            // Time taken to sample = FFT_SAMPLES*samplingPeriod = 4096 * 62us = 253952 us = 0.25s
            for (int i=0;i<FFT_SAMPLES;i++){
                sampleAudio(i, samplingPeriod);
            }
            uint16_t* fft_sample = generateFFT();
            for (int i=2;i<FFT_SAMPLES/2;i++){
                uint32_t sum_bin = ((fft_averages[i]*n)+fft_sample[i]);
                uint32_t average = sum_bin/n;
                if (sum_bin/n>40000){
                    Serial.print(i*1.0 * samplingFrequency/FFT_SAMPLES);
                    Serial.print('\t');
                    Serial.println(average);

                }
                if (average > UINT16_MAX) { // clamp to maximum value
                    average = UINT16_MAX;
                }
                fft_averages[i] = average;
            }
            free(fft_sample);
        }
        
        uint16_t bands[8];
        memset(&bands, 0, sizeof(bands));
        const valuePack data = binFFT(bands, fft_averages, boundaries);
        const double dba = sampleDBA();
        //const double dba = 0.0;
        Serial.println(dba);

        sendData(data, dba);
        //DEBUG: Blocking loop
        sleep(UPDATE_INTERVAL);
    }
}