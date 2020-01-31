/**
 * transmit.h
 * Header file to contain the transmission methods and helpers
 * Copyright (C) Michael Hathaway 2020
 */
void send_loop();
bool sendData(const char* data, int length);
void init_send();