#include <arduinoFFT.h>
#include "config.h"

//TODO: tidy these references up. Priority is speed here but I'd rather not throw pointers everywhere
int bands[9];
int boundaryIndexes[9];
double vReal[samples];
double vImag[samples];
arduinoFFT FFT = arduinoFFT(vReal, vImag, samples, samplingFrequency);
uint16_t currentSample;

const uint8_t amplitude = 150;

#define arrayLen(target) sizeof(target)/sizeof(target[0])

int max(int a, int b)

void generateFFT() {
    for (int i = 2; i < (samples/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array eleement represents a frequency and its value the amplitude.
        if (vReal[i] > 2000) { // Add a crude noise filter, 10 x amplitude or more
            if (i<=boundaryIndexes[0]) bands[0] = max(bands[0], (int)(vReal[i]/amplitude));
            for (int n=1; n < arrayLen(boundaryIndexes); n++){
                if (i > boundaryIndexes[n-1] && i <= boundaryIndexes[n]) bands[n] = max(bands[n], (int)(vReal[i]/amplitude));
            }
        }
    }
    FFT.Compute(FFT_FORWARD);
}

void init_transform() {
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