#include <Arduino.h>
#include <arduinoFFT.h>

#include "config.h"
#include "trans_objects.h"
#include "pack.h"

arduinoFFT FFT = arduinoFFT(vReal, vImag, FFT_SAMPLES, samplingFrequency);

const uint8_t amplitude = 150;
uint16_t currentSample = 0;

#define arrayLen(target) sizeof(target)/sizeof(target[0])

uint16_t max(uint16_t a, uint16_t b){
    //Serial.printf("comparing %d and %d\r\n", a, b);
    return a>b?a:b;
}

valuePack generateFFT(uint16_t bands[9], double boundaries[9]) {
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    float step = 1.0 * samplingFrequency/FFT_SAMPLES;
    for (int i = 2; i < (FFT_SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a bin and its value the amplitude.
        if (vReal[i] > amplitude*10) { // Add a crude noise filter, 10 x amplitude or more
            //Find the big bin this fits in
            for (uint8_t n=0; n<9; n++) { //TODO: determine length dynamically
                const float representedFrequency = step*i;
                float lowerBound = n!=0?boundaries[n-1]:step;
                float upperBound = boundaries[n];
                //Serial.printf("is %f < %f <= %f ?\n", lowerBound, representedFrequency, upperBound);
                if (lowerBound < representedFrequency && representedFrequency <= upperBound) {
                    bands[n] = max(bands[n], (uint16_t)(vReal[i]/amplitude));
                    break; // we found our target, so stop searching
                }
            }
        }
    }

    //DEBUG
    for (int i=0; i<9; i++){
        Serial.printf("%d ", bands[i]);
    }
    Serial.printf("%f ", FFT.MajorPeak());
    Serial.print("\r\n");
    //DBG END
    valuePack output = {bands, FFT.MajorPeak()};
    return output;
}

void init_transform(double* boundaries) {
    //generate the fft frequency boundaries
    uint8_t boundaryCounter = 0;
    for(int i=2; i<FFT_SAMPLES; i++) {
        double frequency = (i * samplingFrequency * 1.0)/ FFT_SAMPLES; // nth bin is n * Fs / N
        if (frequency >= boundaries[boundaryCounter] && boundaryCounter < arrayLen(boundaryIndexes)) {
            boundaryIndexes[boundaryCounter] = i;
            Serial.printf("%f %f %d\r\n", frequency, boundaries[boundaryCounter], boundaryIndexes[boundaryCounter]);
            boundaryCounter++;
        }
    }
    Serial.println(FFT_SAMPLES);
}

void insertValue(int value) {
    vReal[currentSample] = value;
    vImag[currentSample] = 0;
    currentSample++;
    if (currentSample>=FFT_SAMPLES-1) {
        currentSample = currentSample % FFT_SAMPLES;
    }
}