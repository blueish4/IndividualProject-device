#include <arduinoFFT.h>
#include "config.h"
#include "trans_objects.h"

arduinoFFT FFT = arduinoFFT(vReal, vImag, FFT_SAMPLES, samplingFrequency);

const uint8_t amplitude = 150;

#define arrayLen(target) sizeof(target)/sizeof(target[0])

int max(int a, int b){
    return a>b?a:b;
}

uint16_t* generateFFT() {
    static uint16_t bands[9];
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
    for (int i = 2; i < (FFT_SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a frequency and its value the amplitude.
        if (vReal[i] > 1500) { // Add a crude noise filter, 10 x amplitude or more
            if (i<=boundaryIndexes[0]) bands[0] = max(bands[0], (int)(vReal[i]/amplitude));
            for (int n=1; n < arrayLen(boundaryIndexes); n++){
                if (i > boundaryIndexes[n-1] && i <= boundaryIndexes[n]) bands[n] = max(bands[n], (int)(vReal[i]/amplitude));
            }
        }
    }

    //DEBUG
    for (int i=0; i<9; i++){
        Serial.printf("%d ", bands[i]);
    }
    Serial.print('\n');
    //DBG END
    return bands;
}

void init_transform(double* boundaries) {
    //generate the fft frequency boundaries
    uint8_t boundaryCounter = 0;
    for(int i=2; i<FFT_SAMPLES; i++) {
        double frequency = (i * (samplingFrequency/2) * 1.0)/ FFT_SAMPLES;
        if (frequency >= boundaries[boundaryCounter] && boundaryCounter < arrayLen(boundaryIndexes)) {
            boundaryIndexes[boundaryCounter] = i;
            boundaryCounter++;
            Serial.printf("%f %f\n", frequency, boundaries[boundaryCounter-1]);
        }
    }
}

void insertValue(int value) {
    int toUpdate = ++currentSample%FFT_SAMPLES;
    vReal[toUpdate] = value;
    vImag[toUpdate] = 0;
    if (currentSample>=FFT_SAMPLES) {
        currentSample = currentSample % FFT_SAMPLES;
    }
}