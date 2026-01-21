#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "Config.h"
#include "Website.h"
#include "ConcussionMath.h"

// --- Global Objects ---
Adafruit_MPU6050 mpu;
Adafruit_ADXL345_Unified adxl(12345);
ESP8266WebServer server(80);

// --- Global State ---
struct {
    float ax, ay, az;
    float temp;
    String log;
    float prev_wx, prev_wy, prev_wz;
    uint32_t last_sample_us;
} device;

struct {
    bool active = false;
    uint32_t start_ms = 0;
    uint32_t last_ms = 0;
    float peakPLA = 0.0f;
    float peakPRA = 0.0f;
} event;

bool led_impact_on = false;
uint32_t led_timer_ms = 0;

// --- Helpers ---
void logEvent(String msg) {
    Serial.println(msg);
    device.log = msg + "<br>" + device.log;
    if (device.log.length() > 1500) device.log = device.log.substring(0, 1500);
}

void handleData() {
    String json;
    json.reserve(256); // Pre-allocate memory to prevent fragmentation
    json = "{";
    json += F("\"ax\":"); json += String(device.ax, 2);
    json += F(",\"ay\":"); json += String(device.ay, 2);
    json += F(",\"az\":"); json += String(device.az, 2);
    json += F(",\"temp\":"); json += String(device.temp, 1);
    json += F(",\"log\":\""); json += device.log;
    json += F("\"}");
    server.send(200, F("application/json"), json);
}

// --- Initialization ---
void setup() {
    // 1. Hardware Initialization
    Serial.begin(115200);
    pinMode(RED_LED_PIN, OUTPUT);
    digitalWrite(RED_LED_PIN, LOW);

    // 2. I2C Bus Start
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000); // Fast mode for high-frequency sampling

    // 3. Sensor Initialization & Verification
    bool mpu_ok = mpu.begin(MPU_ADDR, &Wire);
    if (mpu_ok) {
        Serial.println(MSG_MPU_FOUND);
    } else {
        Serial.println("MPU6050 missing at " + String(MPU_ADDR, HEX));
    }

    bool adxl_ok = adxl.begin(ADXL_ADDR);
    if (adxl_ok) {
        Serial.println(MSG_ADXL_FOUND);
    } else {
        Serial.println("ADXL375 missing at " + String(ADXL_ADDR, HEX));
    }

    // Critical Halt if sensors fail
    if (!mpu_ok || !adxl_ok) {
        Serial.println(MSG_SENSOR_ERROR);
        while (1) { 
            delay(100); 
            digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN)); // Rapid blink = hardware error
        }
    }

    // 4. Sensor Calibration/Settings
    mpu.setGyroRange(MPU6050_RANGE_2000_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
    // Note: ADXL375 is factory-set to +/- 200g, no range set needed.

    // 5. Wi-Fi Access Point Setup
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    Serial.println(MSG_HOTSPOT_OK);
    Serial.print(MSG_IP_ADDR);
    Serial.println(WiFi.softAPIP());

    // 6. Web Server Endpoints
    server.on("/", []() { 
        server.send(200, "text/html", index_html); 
    });

    server.on("/data", handleData);

    server.on("/toggle", []() { 
        digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN)); 
        server.send(200, "text/plain", "OK"); 
    });

    server.begin();

    // 7. Final State Prep
    device.last_sample_us = micros();
    logEvent(MSG_SYS_ONLINE);
}

// --- Main Loop ---
void loop() {
    // 1. Web Services
    server.handleClient();

    // 2. Sample Timing (Non-blocking)
    uint32_t now_us = micros();
    if (now_us - device.last_sample_us < SAMPLE_PERIOD_US) return;
    
    float dt = (now_us - device.last_sample_us) / 1e6f;
    // Safety check for dt
    if (dt <= 0) dt = (SAMPLE_PERIOD_US / 1e6f);
    device.last_sample_us = now_us;

    // 3. Sensor Acquisition
    sensors_event_t a_ev, g_ev, t_ev;
    adxl.getEvent(&a_ev);           // Get Accel from ADXL375
    mpu.getEvent(&a_ev, &g_ev, &t_ev); // Get Gyro/Temp from MPU6050

    // Update state for web dashboard
    device.ax = a_ev.acceleration.x;
    device.ay = a_ev.acceleration.y;
    device.az = a_ev.acceleration.z;
    device.temp = t_ev.temperature;

    // 4. Physics Calculations
    // Linear acceleration magnitude (minus gravity offset)
    float a_mag_g = (sqrtf(device.ax*device.ax + device.ay*device.ay + device.az*device.az) / G0) - 1.0f;
    if (a_mag_g < 0) a_mag_g = 0;

    // Angular acceleration magnitude (Alpha)
    float alpha_x = (g_ev.gyro.x - device.prev_wx) / dt;
    float alpha_y = (g_ev.gyro.y - device.prev_wy) / dt;
    float alpha_z = (g_ev.gyro.z - device.prev_wz) / dt;
    float alpha_mag = sqrtf(alpha_x*alpha_x + alpha_y*alpha_y + alpha_z*alpha_z);
    
    // Store previous gyro values for next dt calculation
    device.prev_wx = g_ev.gyro.x; 
    device.prev_wy = g_ev.gyro.y; 
    device.prev_wz = g_ev.gyro.z;

    uint32_t now_ms = millis();

    // 5. LED Control Logic
    if (led_impact_on && (now_ms - led_timer_ms > 1000)) {
        digitalWrite(RED_LED_PIN, LOW);
        led_impact_on = false;
    }

    // 6. Impact Detection Trigger
    bool can_trigger = (now_ms - event.last_ms > REFRACTORY_MS);
    
    if (!event.active && can_trigger && a_mag_g >= TRIGGER_G) {
        event.active = true;
        event.start_ms = now_ms;
        event.peakPLA = a_mag_g;
        event.peakPRA = alpha_mag;
        
        digitalWrite(RED_LED_PIN, HIGH);
        led_impact_on = true;
        led_timer_ms = now_ms;
        
        logEvent(MSG_IMPACT_DETECTED);
    }

    // 7. Event Processing (Windowing)
    if (event.active) {
        // Track the highest peaks found during the 80ms window
        if (a_mag_g > event.peakPLA) event.peakPLA = a_mag_g;
        if (alpha_mag > event.peakPRA) event.peakPRA = alpha_mag;

        // Close window and report
        if (now_ms - event.start_ms >= WINDOW_MS) {
            event.active = false;
            event.last_ms = now_ms;

            float cp = combinedProbability(event.peakPLA, event.peakPRA);
            
            // Build and log the report string
            String report = MSG_EVENT_REPORT + String(event.peakPLA, 1) + "g" + 
                           MSG_RISK_LABEL + riskLabel(cp);
            
            logEvent(report);

            if (cp >= CP_MED) {
                logEvent(MSG_CHECK_PLAYER);
            }
        }
    }
}