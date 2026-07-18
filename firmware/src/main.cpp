/**
 * @file main.cpp
 * @brief Main entry point for the ventilator TFT interface
 * 
 * This file initializes the DisplayManager and FreeRTOS tasks.
 * The actual motor control, PID, sensor reading, and ventilation algorithms
 * are handled by a separate developer.
 */

#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <math.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "DisplayManager.h"

// Global DisplayManager instance
DisplayManager displayManager;

// Configure these values before testing with real hardware.
const char* WIFI_SSID = "TON_WIFI";
const char* WIFI_PASSWORD = "TON_MOT_DE_PASSE";
// Broker MQTT public HiveMQ (compatible avec le dashboard web)
const char* MQTT_HOST = "broker.hivemq.com";
const uint16_t MQTT_PORT = 1883;

const uint32_t DATA_INTERVAL_MS = 100;
const uint32_t WIFI_RETRY_INTERVAL_MS = 5000;
const uint32_t MQTT_RETRY_INTERVAL_MS = 3000;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Task handles
TaskHandle_t displayTaskHandle = NULL;
unsigned long lastDataMs = 0;
unsigned long lastWifiAttemptMs = 0;
unsigned long lastMqttAttemptMs = 0;

const char* modeToString(VentilationMode mode) {
    switch (mode) {
        case MODE_PCV:
            return "PCV";
        case MODE_VAC:
            return "VAC";
        case MODE_VCV:
        default:
            return "VCV";
    }
}

VentilationMode stringToMode(const char* mode) {
    if (strcmp(mode, "PCV") == 0) {
        return MODE_PCV;
    }
    if (strcmp(mode, "VAC") == 0) {
        return MODE_VAC;
    }
    return MODE_VCV;
}

void connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    if (strcmp(WIFI_SSID, "TON_WIFI") == 0) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastWifiAttemptMs < WIFI_RETRY_INTERVAL_MS) {
        return;
    }

    lastWifiAttemptMs = now;
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    // Topic config compatible avec le dashboard web
    if (strcmp(topic, "respirateur-medvent/config") != 0) {
        return;
    }

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        Serial.print("MQTT config parse error: ");
        Serial.println(error.c_str());
        return;
    }

    VentilatorSettings settings = displayManager.getSettings();
    settings.mode = stringToMode(doc["mode"] | modeToString(settings.mode));
    settings.fr = doc["fr"] | settings.fr;
    settings.volume = doc["volume"] | settings.volume;
    settings.peep = doc["peep"] | settings.peep;
    settings.ie_ratio = doc["ie_ratio"] | settings.ie_ratio;
    displayManager.setSettings(settings);

    Serial.println("Received respirateur-medvent/config from dashboard");
}

void connectMQTT() {
    if (WiFi.status() != WL_CONNECTED || mqttClient.connected()) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastMqttAttemptMs < MQTT_RETRY_INTERVAL_MS) {
        return;
    }

    lastMqttAttemptMs = now;
    String clientId = "ventilator-esp32-" + String((uint32_t)ESP.getEfuseMac(), HEX);

    Serial.print("Connecting to MQTT broker: ");
    Serial.println(MQTT_HOST);
    if (mqttClient.connect(clientId.c_str())) {
        mqttClient.subscribe("respirateur-medvent/config");
        Serial.println("MQTT connected and subscribed to respirateur-medvent/config");
    } else {
        Serial.print("MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
    }
}

// ============================================================================
// SIMULATION DES DONNÉES RESPIRATOIRES
// ============================================================================
// NOTE IMPORTANTE : Cette fonction génère des données simulées pour les tests.
// Pour le prototype physique, remplacer cette fonction par readRealSensors()
// qui lit les capteurs réels (MPXV7025DP pour pression, SFM3000 pour débit).
// Voir GUIDE_TRANSITION.md pour les détails d'intégration des capteurs réels.
// ============================================================================
VentilatorData generateSimulatedData() {
    VentilatorSettings settings = displayManager.getSettings();
    const float safeFr = settings.fr > 0.1f ? settings.fr : 15.0f;
    const float periodMs = 60000.0f / safeFr;
    const float cycle = fmodf((float)millis(), periodMs) / periodMs;
    const float wave = sinf(cycle * 2.0f * PI);
    const float inspiration = cycle < settings.ie_ratio ? 1.0f : 0.35f;

    VentilatorData data;
    
    // SIMULATION : Remplacer par lecture capteur MPXV7025DP
    // Exemple : data.pressure = readPressureSensor();
    data.pressure = max(0.0f, settings.peep + 18.0f * max(0.0f, wave) * inspiration);
    
    // SIMULATION : Remplacer par lecture capteur SFM3000 (I²C)
    // Exemple : data.flow = readFlowSensor();
    data.flow = 55.0f * wave * inspiration;
    
    // SIMULATION : Calculer par intégration du débit réel
    // Exemple : data.volume = integrateFlow(data.flow);
    data.volume = max(0.0f, settings.volume * sinf(cycle * PI));
    
    data.fr = settings.fr;
    data.peep = settings.peep;
    data.mode = settings.mode;
    data.alarm = data.pressure > 30.0f;
    
    // SIMULATION : Remplacer par lecture batterie réelle
    data.battery = 95.0f;
    data.wifi = WiFi.status() == WL_CONNECTED;
    return data;
}

void publishVentilatorData(const VentilatorData& data) {
    if (!mqttClient.connected()) {
        return;
    }

    StaticJsonDocument<256> doc;
    doc["pressure"] = data.pressure;
    doc["flow"] = data.flow;
    doc["volume"] = data.volume;
    doc["fr"] = data.fr;
    doc["peep"] = data.peep;
    doc["mode"] = modeToString(data.mode);
    doc["alarm"] = data.alarm;
    doc["battery"] = data.battery;
    doc["wifi"] = data.wifi;

    char payload[256];
    serializeJson(doc, payload, sizeof(payload));
    // Topic data compatible avec le dashboard web
    mqttClient.publish("respirateur-medvent/data", payload);
}

/**
 * @brief Display update task for FreeRTOS
 * @param pvParameters Task parameters (unused)
 */
void displayTask(void *pvParameters) {
    while (1) {
        // Update display - this will be called by the main firmware
        // with actual sensor data
        displayManager.update();
        
        // 50ms refresh rate for smooth UI
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Ventilator TFT Interface Starting...");
    
    // Initialize DisplayManager
    displayManager.begin();
    
    // Create display task
    xTaskCreate(
        displayTask,
        "DisplayTask",
        8192,
        NULL,
        2,
        &displayTaskHandle
    );

    mqttClient.setServer(MQTT_HOST, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    
    Serial.println("Display Manager initialized");
    Serial.println("Edit WIFI_SSID, WIFI_PASSWORD, and MQTT_HOST in src/main.cpp before MQTT testing.");
}

void loop() {
    connectWiFi();
    connectMQTT();
    mqttClient.loop();

    const unsigned long now = millis();
    if (now - lastDataMs >= DATA_INTERVAL_MS) {
        lastDataMs = now;
        VentilatorData data = generateSimulatedData();
        displayManager.setData(data);
        publishVentilatorData(data);
    }

    vTaskDelay(pdMS_TO_TICKS(10));
}
