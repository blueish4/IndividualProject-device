#include <Arduino.h>
#include <arduinoFFT.h>

#include "config.h"
#include "trans_objects.h"
#include "pack.h"

arduinoFFT FFT = arduinoFFT(vReal, vImag, FFT_SAMPLES, samplingFrequency);

const uint8_t amplitude = 150;
uint16_t currentSample = 0;

#define arrayLen(target) sizeof(target)/sizeof(target[0])

int max(int a, int b){
    return a>b?a:b;
}

valuePack generateFFT(uint16_t bands[9]) {
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();

    int bandIndex = 0;
    for (int i = 2; i < (FFT_SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > 1500) { // Add a crude noise filter, 10 x amplitude or more
        bands[bandIndex] = max(bands[bandIndex], (int)(vReal[i]/amplitude));
            if (i > boundaryIndexes[bandIndex]) bandIndex++;
        }
    }

    //DEBUG
    for (int i=0; i<9; i++){
        Serial.printf("%d ", bands[i]);
    }
    Serial.printf("%f ", FFT.MajorPeak());
    Serial.print('\n');
    //DBG END
    valuePack output = {bands, FFT.MajorPeak()};
    return output;
}

void init_transform(double* boundaries) {
    //generate the fft frequency boundaries
    uint8_t boundaryCounter = 0;
    for(int i=2; i<FFT_SAMPLES; i++) {
        double frequency = (i * samplingFrequency * 1.0)/ FFT_SAMPLES;
        if (frequency >= boundaries[boundaryCounter] && boundaryCounter < arrayLen(boundaryIndexes)) {
            boundaryIndexes[boundaryCounter] = i;
            boundaryCounter++;
            Serial.printf("%f %f\n", frequency, boundaries[boundaryCounter-1]);
        }
    }
}

void insertValue(int value) {
    vReal[currentSample] = value;
    vImag[currentSample] = 0;
    currentSample++;
    if (currentSample>=FFT_SAMPLES-1) {
        currentSample = currentSample % FFT_SAMPLES;
    }
}