# 🛡️ Impact Monitor: ESP8266 Concussion Sensor

![Status](https://img.shields.io/badge/Status-Stable-success)
![Platform](https://img.shields.io/badge/Platform-ESP8266-blue)
![Framework](https://img.shields.io/badge/Framework-Arduino%2FPlatformIO-orange)
![License](https://img.shields.io/badge/License-MIT-green)

An intelligent, IoT-enabled impact monitoring system designed to detect high-G head impacts in real-time. By combining linear acceleration (**ADXL375**) and angular velocity (**MPU6050**), the system calculates the **Combined Probability** of a concussion and alerts staff via a localized web dashboard and onboard visual indicators.

---

## 📑Table of Contents

1. [System full Overview](#system-overview) 
2. [Hardware Requirements](#hardware-requirements)
3. [Wiring & Connections](#wiring--connections)
4. [Software Architecture](#software-architecture)
5. [Installation & Setup](#installation--setup)
6. [The Physics: Concussion Math](#the-physics-concussion-math)
7. [Web Dashboard](#web-dashboard)
8. [Safety Disclaimer](#safety-disclaimer)

---

## 🛰️System Overview

The Impact Monitor functions as a standalone **Wi-Fi Access Point**. It samples sensor data at high frequencies (~800Hz) to ensure that sub-millisecond impact peaks are captured accurately.

- **Primary Sensor:** ADXL375 (±200g range) for Peak Linear Acceleration (PLA).
- **Secondary Sensor:** MPU6050 for Peak Rotational Acceleration (PRA).
- **Processing:** ESP8266 (D1 Mini) calculates risk thresholds locally.
- **Interface:** Mobile-responsive Web Dashboard accessible via any smartphone browser.

---

## 🔌Hardware Requirements

### Component List

| Component | Purpose | Details |
| :--- | :--- | :--- |
| **Wemos D1 Mini** | Logic & Wi-Fi | ESP8266 based microcontroller |
| **ADXL375** | High-G Accel | Measures up to 200g impacts |
| **MPU6050** | Gyroscope | Measures rotational velocity & temperature |
| **Onboard LED** | Visual Alert | Indicators for impact and hardware errors |

---

## ⚡Wiring & Connections

The sensors communicate over the **I2C Bus**. On the D1 Mini, the default connections are:

| Pin Name | ESP8266 Pin | Function |
| :--- | :--- | :--- |
| **SDA** | GPIO4 (D2) | I2C Data |
| **SCL** | GPIO5 (D1) | I2C Clock |
| **LED** | GPIO0 (D3) | Visual Indicator |

> **Note:** Ensure your sensors are supplied with the correct voltage (typically 3.3V) from the D1 Mini.

---

## 📂Software Architecture
```text
├── include/
│   ├── Config.h          # Hardware pins, Wi-Fi credentials, and constants
│   ├── Website.h         # HTML/CSS/JS dashboard stored in PROGMEM
│   ├── ConcussionMath.h  # Risk probability and label algorithms
│   ├── Strings.h         # Localization and UI text definitions
│   ├── Messages.h        # System status messages and logging definitions
│   └── I2CManager.h      # Abstraction layer for I2C sensor communication
├── src/
│   ├── main.cpp          # Main setup/loop and Global State management
│   └── I2CManager.cpp    # Implementation of sensor data retrieval
└── platformio.ini        # Dependency and board configuration 
```

The project is built on the **PlatformIO** framework and divides functionality into logical states:

### 1. Global State Management
* **`device` struct:** Acts as the central data store for real-time sensor readings (`ax`, `ay`, `az`, `temp`) and system status logs.
* **`event` struct:** Manages the high-speed "Impact Window." When a threshold is crossed, the system locks into an active state for **80ms** to record the peak magnitude of the impact.

### 2. Localization & Interface
* **Web Server:** Hosted directly on the ESP8266, serving a responsive HTML/JS dashboard stored in program memory.
* **Localization:** System messages and dashboard text are separated to allow for easy translation and updates.

---

## 🚀Installation & Setup

1.  **Prerequisites:**
    * Install [Visual Studio Code](https://code.visualstudio.com/).
    * Install the [PlatformIO](https://platformio.org/) extension.

2.  **Library Dependencies:** The following libraries are required (automatically handled by `platformio.ini`):
    * `Adafruit MPU6050`
    * `Adafruit ADXL345` (Supports ADXL375)
    * `Adafruit Unified Sensor`

3.  **Upload:** Connect your D1 Mini via USB and click the **PlatformIO: Upload** (➡️) button.

4.  **Accessing the Dashboard:**
    * Search for Wi-Fi Network: `Impact-Monitor-AP`
    * Password: `12345678`
    * Open your browser and navigate to: `http://192.168.4.1`

---

## 🧪The Physics: Concussion Math

The system doesn't just measure "how hard" a hit was; it measures **risk**.

### 1. Linear Acceleration (PLA)
Measured in $g$, extracted from the ADXL375. We subtract gravity ($1g$) from the total magnitude to find the "Impact G".

### 2. Rotational Acceleration (PRA)
Calculated by taking the derivative of the Gyroscope data from the MPU6050 over time ($\Delta t$).

### 3. Combined Probability (CP)
The code utilizes a probability function that combines PLA and PRA to determine the likelihood of a concussive event.

* **Low Risk:** PLA < 10g or CP < 0.20
* **Medium Risk:** CP ≥ 0.20 (⚠️ Yellow Warning)
* **High Risk:** CP ≥ 0.50 (🚨 Red Warning)

---

## 💻Web Dashboard

The built-in webserver provides a real-time interface:

* **Live Telemetry:** View current X, Y, Z forces and internal temperature.
* **System Log:** A scrolling terminal window showing impact reports and safety warnings.
* **LED Override:** A manual button to test the physical onboard LED.

---

## 🤝Contributing

Contributions, issues, and feature requests are welcome!
Feel free to check the [issues page](https://github.com/marnix707/Headbang/issues).

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

---

## ⚠️Safety Disclaimer

This device is a **prototype** and is intended for research and educational purposes only. It is **not** a certified medical device.

* A "Low Risk" reading does not guarantee the absence of a concussion.
* A "High Risk" reading should be followed by a professional medical evaluation immediately.
* Always follow official sports safety protocols (e.g., SCAT5).