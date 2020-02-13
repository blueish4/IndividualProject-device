#include <Arduino.h>
#include <arduinoFFT.h>

#include "config.h"
#include "trans_objects.h"
#include "pack.h"
#include "debug.h"

arduinoFFT FFT = arduinoFFT(vReal, vImag, FFT_SAMPLES, samplingFrequency);

const uint8_t amplitude = 150;
uint16_t currentSample = 0;

#define arrayLen(target) sizeof(target)/sizeof(target[0])

uint16_t max(uint16_t a, uint16_t b){
    return a>b?a:b;
}

// Calculate the weighting value to be multiplied by the frequency amplitude
double get_weighting(double frequency) {
    // the formula for Ra(f) if from wikipedia.
    // Return value is 20log10(Ra(f)) + 2 (to set weighting reference to standard 0dB at 1kHz)
    // commonly used in the transfer function, so cache
    double f_squared = pow(frequency, 2);
    double big_square = pow(12194, 2);
    double f_big_square = f_squared*big_square;

    double Ra = (f_big_square*f_squared)/((f_squared+pow(20.6,2))*sqrt((f_squared+pow(107.7, 2))*(f_squared+pow(737.9,2)))*f_big_square);
    return Ra*1000;  // amplify by arbitrary amount
}

valuePack binFFT(uint16_t bands[8], uint16_t* fft, double* boundaries) {
    float step = 1.0 * samplingFrequency/FFT_SAMPLES;
    peak maxPeak = {0, step}; // Initialise struct with 0 value
    for (int i = 2; i < (FFT_SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a bin and its value the amplitude.
        const float representedFrequency = step*i;

        if (maxPeak.value<fft[i]) {
            Serial.printf("Next highest: %d\t%f\n", fft[i],representedFrequency);
            maxPeak.frequency = representedFrequency;
            maxPeak.value = fft[i];
        }
        //Find the big bin this fits in
        
        for (uint8_t n=0; n<8; n++) { //TODO: determine length dynamically
            float lowerBound = n!=0?boundaries[n-1]:step;
            float upperBound = boundaries[n];
            //Serial.printf("is %f < %f <= %f  sending %d?\n", lowerBound, representedFrequency, upperBound, (uint16_t)(vReal[i]/amplitude));
            if (lowerBound < representedFrequency && representedFrequency <= upperBound) {
                bands[n] = max(bands[n], (uint16_t) round(fft[i]));
                break; // we found our target, so stop searching
            }
        }
    }

    //DEBUG
    for (int i=0; i<8; i++){
        Serial.printf("%d ", bands[i]);
    }
    Serial.printf("%f %d", maxPeak.frequency, maxPeak.value);
    Serial.print("\r\n");
    //DBG END
    valuePack output = {bands, maxPeak};
    return output;
}

// Generates the weighted FFT values, returns values in heap allocated memory
uint16_t* generateFFT() {
    // Now compute FFT
    FFT.Windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(FFT_FORWARD);
    FFT.ComplexToMagnitude();
    uint16_t* weighted = (uint16_t*) malloc(sizeof(uint16_t)*(FFT_SAMPLES/2));
    // Weight the bins
    float step = 1.0 * samplingFrequency/FFT_SAMPLES;
    for (int i = 2; i < (FFT_SAMPLES/2); i++){ // Don't use sample 0 and only first SAMPLES/2 are usable. Each array element represents a bin and its value the amplitude.
        const float representedFrequency = step*i;
        // apply weighting scale factor
        const auto weightedReal = vReal[i]*get_weighting(representedFrequency);
        weighted[i] = (uint16_t)round(weightedReal);
        //Serial.printf("%f\t%d\t%f\n", weightedReal, weighted[i], representedFrequency);
    }
    return weighted;
}

void init_transform(double* boundaries) {
    // Initialise the variables nicely
    memset(&vReal, 0, sizeof(vReal));
    memset(&vImag, 0, sizeof(vImag));

    // warn on step width inadiquacy
    float stepWidth = 2.0*samplingFrequency/FFT_SAMPLES;
    if (stepWidth > boundaries[0]) {
        dbg_print("WARN: step too big");
    }

    // NOT USED CURRENTLY
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
    Serial.println(1.0*samplingFrequency/FFT_SAMPLES);
    Serial.println(FFT_SAMPLES);
}

void insertValue(int value) {
    int place = currentSample;
    currentSample = (++place)%FFT_SAMPLES;
    insertValue(value, place);
}
void insertValue(int value, int place) {
    vReal[place] = value;
    vImag[place] = 0;
}