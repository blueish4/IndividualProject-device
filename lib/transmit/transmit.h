/**
 * transmit.h
 * Header file to contain the transmission methods and helpers
 * Copyright (C) blueish 2020
 */
#include "pack.h"
void send_loop();
bool sendData(const char* data, int length);
void sendData(valuePack p, double dba);
void init_send();