#ifndef SCAN_I2C_BUS_H
#define SCAN_I2C_BUS_H

#include <Arduino.h>
#include <Wire.h>

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
#endif