#ifndef STRINGS_EN_H
#define STRINGS_EN_H

#include <Arduino.h>

// --- System Initialization ---
const String MSG_SYS_START      = "System started (ESP8266)...";
const String MSG_SYS_ONLINE     = "System online.";
const String MSG_HOTSPOT_OK     = "Hotspot started!";
const String MSG_IP_ADDR        = "IP address for browser: ";
const String MSG_SENSOR_ERROR   = "Sensor Init Failed! Check wiring.";
const String MSG_MPU_FOUND      = "MPU6050 Found!";
const String MSG_ADXL_FOUND     = "ADXL375 Found!";

// --- Impact Events ---
const String MSG_IMPACT_DETECTED = "--- IMPACT DETECTED! ---";
const String MSG_CHECK_PLAYER    = "⚠️ CHECK PLAYER!";
const String MSG_EVENT_REPORT    = "Event: PLA=";
const String MSG_RISK_LABEL      = " Risk=";

// --- Web Interface ---
const String MSG_WAITING_CONN    = "Waiting for connection...";
const String MSG_CONN_ERROR      = "Connection Error";

#endif