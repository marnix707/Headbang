# 🛡️ Impact Monitor: ESP8266 Concussion Sensor

![Status](https://img.shields.io/badge/Status-Stable-success)
![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino/PlatformIO-orange)

An intelligent, IoT-enabled impact monitoring system designed to detect high-G head impacts in real-time. By combining linear acceleration (ADXL375) and angular velocity (MPU6050), the system calculates the **Combined Probability** of a concussion and alerts staff via a localized web dashboard and onboard visual indicators.

---

## 📑 Table of Contents
1. [System Overview](#system-overview)
2. [Hardware Requirements](#hardware-requirements)
3. [Software Architecture](#software-architecture)
4. [Installation & Setup](#installation--setup)
5. [The Physics: Concussion Math](#the-physics-concussion-math)
6. [Global State Management](#global-state-management)
7. [Web Dashboard](#web-dashboard)
8. [Safety Disclaimer](#safety-disclaimer)

---

## 🛰️System Overview

The Impact Monitor functions as a standalone **Wi-Fi Access Point**. It samples sensor data at high frequencies (~800Hz) to ensure that sub-millisecond impact peaks are captured accurately.

* **Primary Sensor:** ADXL375 (±200g range) for Peak Linear Acceleration (PLA).
* **Secondary Sensor:** MPU6050 for Peak Rotational Acceleration (PRA).
* **Processing:** ESP8266 (D1 Mini) calculates risk thresholds locally.
* **Interface:** Mobile-responsive Web Dashboard accessible via any smartphone browser.

---

## 🔌Hardware Requirements

### Component List
| Component | Purpose | Details |
| :--- | :--- | :--- |
| **Wemos D1 Mini** | Logic & Wi-Fi | ESP8266 based microcontroller |
| **ADXL375** | High-G Accel | Measures up to 200g impacts |
| **MPU6050** | Gyroscope | Measures rotational velocity & temperature |
| **Red LED** | Visual Alert | Indicators for impact and hardware errors |

### Wiring Configuration
The sensors communicate over the **I2C Bus**. On the D1 Mini, the defaults are:
* **SDA:** GPIO4 (D2)
* **SCL:** GPIO5 (D1)
* **LED:** GPIO0 (D3)

---

## 📂Software Architecture

The project follows a modular C++ structure for maximum readability and ease of maintenance.

```text
├── include/
│   ├── Config.h          # Hardware pins, Wi-Fi credentials, and constants
│   ├── Website.h         # HTML/CSS/JS dashboard stored in PROGMEM
│   ├── ConcussionMath.h  # Risk probability and label algorithms
│   └── Strings_en.h      # Localization (English text logs)
├── src/
│   └── main.cpp          # Main setup/loop with Global State management
└── platformio.ini        # Dependency and board configuration

```

---

## 🚀Installation & Setup

1. **Prerequisites:** Install [Visual Studio Code](https://code.visualstudio.com/) and the [PlatformIO](https://platformio.org/) extension.
2. **Library Dependencies:** The following libraries are required (automatically handled by PlatformIO):
* `Adafruit MPU6050`
* `Adafruit ADXL345` (Supports ADXL375)
* `Adafruit Unified Sensor`


3. **Upload:** Connect your D1 Mini via USB and click the **PlatformIO: Upload** button.
4. **Accessing the Dashboard:**
* Search for Wi-Fi: `Impact-Monitor-AP`
* Password: `12345678`
* Open browser and go to: `http://192.168.4.1`



---

## 🧪The Physics: Concussion Math

The system doesn't just measure "how hard" a hit was; it measures **risk**.

### 1. Linear Acceleration (PLA)

Measured in , extracted from the ADXL375. We subtract gravity () from the total magnitude to find the "Impact G".

### 2. Rotational Acceleration (PRA)

Calculated by taking the derivative of the Gyroscope data from the MPU6050 over time ():


### 3. Combined Probability (CP)

The code utilizes a probability function that combines PLA and PRA to determine the likelihood of a concussive event.

* **Low Risk:** PLA < 10g or CP < 0.20
* **Medium Risk:** CP ≥ 0.20 (⚠️ Yellow Warning)
* **High Risk:** CP ≥ 0.50 (🚨 Red Warning)

---

## 🧠Global State Management

The code uses structured global states to keep the memory footprint organized:

* **`device` struct:** Stores current real-time sensor data (ax, ay, az, temp) and the system log string. This is what the Web Dashboard reads every second.
* **`event` struct:** Handles the "Impact Window." When a hit is detected, the system enters an "Active" state for 80ms to capture the absolute highest peaks before reporting.

---

## 💻Web Dashboard

The built-in webserver provides a real-time interface:

* **Live Telemetry:** View current X, Y, Z forces and internal temperature.
* **System Log:** A scrolling terminal window showing impact reports and safety warnings.
* **LED Override:** A manual button to test the physical onboard LED.

---

## ⚠️Safety Disclaimer

This device is a **prototype** and is intended for research and educational purposes only. It is **not** a certified medical device.

* A "Low Risk" reading does not guarantee the absence of a concussion.
* A "High Risk" reading should be followed by a professional medical evaluation immediately.
* Always follow official sports safety protocols (e.g., SCAT5).

---

```
