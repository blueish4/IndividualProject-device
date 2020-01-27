#ifndef TRANS_OBJECTS_H_
#define TRANS_OBJECTS_H_

#define samplingFrequency 16500
#define FFT_SAMPLES 2048

int boundaryIndexes[9];
double vReal[FFT_SAMPLES];
double vImag[FFT_SAMPLES];
uint16_t currentSample = 0;
#endif