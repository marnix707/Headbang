#ifndef SCAN_I2C_BUS_H
#define SCAN_I2C_BUS_H

#include <Arduino.h>
#include <Wire.h>
#include "Config.h"

void scanI2CBus() {
    Serial.println(F("--- I2C Bus Scan ---"));
    byte error, address;
    int nDevices = 0;

    for (address = 1; address < 127; address++) {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0) {
            Serial.print(F("Device found at address 0x"));
            if (address < 16) Serial.print("0");
            Serial.println(address, HEX);
            nDevices++;
        }
    }
    if (nDevices == 0) {
        Serial.println(F("No I2C devices found. Check your SDA/SCL wiring!"));
    }
    Serial.println(F("--------------------"));
}


void clearI2CBus() {
    pinMode(SDA_PIN, INPUT_PULLUP);
    pinMode(SCL_PIN, OUTPUT);

    // Send 9 clock pulses to force any hung sensor to release SDA
    for (int i = 0; i < 9; i++) {
        digitalWrite(SCL_PIN, LOW);
        delayMicroseconds(5);
        digitalWrite(SCL_PIN, HIGH);
        delayMicroseconds(5);
    }

    // Send a STOP condition
    pinMode(SDA_PIN, OUTPUT);
    digitalWrite(SDA_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(SCL_PIN, HIGH);
    delayMicroseconds(5);
    digitalWrite(SDA_PIN, HIGH);
}

#endif