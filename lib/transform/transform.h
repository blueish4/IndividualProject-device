#include "pack.h"
#define samplingFrequency 16500
#define FFT_SAMPLES 4096
valuePack generateFFT(uint16_t bands[8], double boundaries[8]);
void init_transform(double* boundaries);
void insertValue(int value);
void insertValue(int value, int place);