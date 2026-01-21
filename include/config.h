#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "Strings_en.h"

// --- Wi-Fi AP Settings ---
const char* AP_SSID = "Impact-Monitor-AP";
const char* AP_PASSWORD = "12345678";

// --- Hardware Settings ---
#define RED_LED_PIN 0      // GPIO0 (D3 on D1 Mini)
#define SDA_PIN D2
#define SCL_PIN D1

// --- I2C Addresses ---
#define MPU_ADDR  0x68
#define ADXL_ADDR 0x53

// --- Impact Parameters ---
const uint32_t SAMPLE_PERIOD_US = 1250;  // ~800Hz
const float TRIGGER_G           = 10.0f;
const uint16_t WINDOW_MS        = 80;
const uint16_t REFRACTORY_MS    = 300;
const float G0                  = 9.80665f;

// --- Risk Thresholds ---
const float CP_MED  = 0.20f;
const float CP_HIGH = 0.50f;

#endif