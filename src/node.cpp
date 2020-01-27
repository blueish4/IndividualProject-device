/**
 * Main program for ESP32 to monitor noise type and loudness
 * Copyright (C) blueish 2020
 */
#include <Arduino.h>
#include <WiFi.h>

#include "arduinoFFT.h"

#include "transmit.h"
#include "debug.h"
#include "transform.h"
#include "cfg.h"

double boundaries[9] = {31.5, 63.0, 125.0, 250.0, 500.0, 1000.0, 2000.0, 4000.0, 8000.0};
unsigned long lastSample = 0;
// retrieves an audio sample and puts it in the queue
// can be moved to an interrupt if required
void sampleAudio(){
    // block until we have waited long enough
    // this may undersample, but can't oversample
    while(micros()<lastSample + (1E6)/16500) {}
    //For the moment, this is analog over pin 36
    lastSample = micros();
    insertValue(analogRead(36));
}

void setup(){
    init_dbg();
    init_send();
    init_transform(boundaries);
}

void loop(){
    int lastMillis = 0;
    while(true) {
        //sampleAudio();
        //Serial.println(vReal[currentSample]);
        for (int i=0;i<2048;i++){
            sampleAudio();
        }
        auto* data = generateFFT();
        send_loop();

        // publish a message roughly every second (+ sample and calculate time).
        // this will end up junking a lot of data if sample time << 1 second
        if (millis() - lastMillis > 6000) {
            lastMillis = millis();
            std::string stringified = "";
            for(int i=0;i<9;i++) {  //TODO: prune magic number here
                stringified += to_string(data[i]) + " ";
                data[i] = 0;
            }
            // stringify array, TODO make a binary transaction
            sendData(String(stringified.c_str()));
        }
    }
}