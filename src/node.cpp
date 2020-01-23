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

// retrieves an audio sample and puts it in the queue
// can be moved to an interrupt if required
void sampleAudio(){
    //For the moment, this is analog over pin 36
    double sampleStart = micros();
    vReal[(++currentSample%samples)] = analogRead(36);
    if (currentSample>=samples) {
        currentSample = currentSample % samples;
    }
    while(micros()-sampleStart<(1E6)/samplingFrequency){

    }
}

void setup(){
    init_dbg();
    init_send();
    init_transform();
}

void loop(){
    int lastMillis = 0;
    while(true) {
        //sampleAudio();
        //Serial.println(vReal[currentSample]);
        for (int i=0;i<samples;i++){
            sampleAudio();
        }
        generateFFT();
        send_loop();

        // TODO: replace with your code
        // publish a message roughly every second.
        if (millis() - lastMillis > 6000) {
            lastMillis = millis();
            //publishTelemetry(mqttClient, "/sensors", getDefaultSensor());
            sendData(WiFi.RSSI());
        }
    }
}