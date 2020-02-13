#include "pack.h"
#define samplingFrequency 40000
#define FFT_SAMPLES 2048
uint16_t* generateFFT();
valuePack binFFT(uint16_t bands[8], uint16_t* fft, double* boundaries);
void init_transform(double* boundaries);
void insertValue(int value);
void insertValue(int value, int place);