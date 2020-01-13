/**
 * config.h
 * Allows features to be enabled and disabled, as well as specifying built-in config options
 * Copyright (C) 2020 blueish
 */

#ifndef CFG_H_
#define CFG_H_

#ifndef DEBUG
#define DEBUG 1
#endif

// Transmit every n seconds
#define UPDATE_INTERVAL 60

// Enable only one channel (868.1MHz), or all channels
#define LORA_SINGLE_CHANNEL 0
#endif