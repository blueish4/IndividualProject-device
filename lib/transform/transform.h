#include "pack.h"
#define samplingFrequency 40000
#define FFT_SAMPLES 2048
valuePack generateFFT(uint16_t bands[8], double boundaries[8]);
void init_transform(double* boundaries);
void insertValue(int value);