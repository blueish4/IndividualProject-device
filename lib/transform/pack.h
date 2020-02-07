#ifndef PACK_H_
#define PACK_H_
struct majorPeak {
    uint16_t value;
    double frequency;
} typedef peak;

struct valuePack {
    uint16_t* frequencies;
    peak majorPeak;
} typedef values;
#endif