/**
 * Main program for ESP32 to monitor noise type and loudness
 * Copyright (C) blueish 2020
 */
#include <Arduino.h>
#include "arduinoFFT.h"

#include "transmit.h"
#include "debug.h"

#define samplingFrequency 16500
#define samples 2048
const uint8_t amplitude = 150;

double boundaries[9] = {31.5, 63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0};
int bands[9];
int boundaryIndexes[9];
arduinoFFT FFT = arduinoFFT();
double vReal[samples];
double vImag[samples];
uint16_t currentSample;


#define arrayLen(target) sizeof(target)/sizeof(target[0])

// retrieves an audio sample and puts it in the queue
// can be moved to an interrupt if required
void sampleAudio(){
    //For the moment, this is analog over pin 36
    double sampleStart = micros();
    vReal[(++currentSample%samples)] = analogRead(36);
    if (currentSample>=samples) {
        currentSample = currentSample % samples;
    }
    while(micros()-sampleStart<(1E6)/samplingFrequency){

    }
}

void generateFFT(){
    for (int i = 2; i < (samples/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
        if (vReal[i] > 2000) { // Add a crude noise filter, 10 x amplitude or more
            if (i<=boundaryIndexes[0]) bands[0] = max(bands[0], (int)(vReal[i]/amplitude));
            for (int n=1; n < arrayLen(boundaryIndexes); n++){
                if (i > boundaryIndexes[n-1] && i <= boundaryIndexes[n]) bands[n] = max(bands[n], (int)(vReal[i]/amplitude));
            }
        }
    }
}

void setup(){
    Serial.begin(115200);
    init_dbg();
    init_send();
    //generate the fft frequency boundaries
    uint8_t boundaryCounter = 0;
    for(int i=2; i<samples; i++) {
        double frequency = (i * (samplingFrequency/2) * 1.0)/ samples;
        if (frequency >= boundaries[boundaryCounter] && boundaryCounter < arrayLen(boundaryIndexes)) {
            boundaryIndexes[boundaryCounter] = i;
            boundaryCounter++;
            Serial.printf("%f %f\n", frequency, boundaries[boundaryCounter-1]);
        }
    }
}

void loop(){
    while(true) {
        //sampleAudio();
        //Serial.println(vReal[currentSample]);
        for (int i=0;i<samples;i++){
            sampleAudio();
        }
        generateFFT();
        for(int i=0;i<arrayLen(bands);i++){
            Serial.print(bands[i]);
            Serial.print(" ");
            bands[i] = 0;
        }
        Serial.print("\n");
        delay(2000);
    }
}