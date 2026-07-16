/**
 * @file DisplayManager.h
 * @brief DisplayManager class for managing the TFT interface with LVGL
 * 
 * This class handles all UI operations for the ventilator display including:
 * - Main screen with metrics and status indicators
 * - Settings screen for ventilation parameters
 * - Alarms screen with alarm history
 * - History screen with event log
 * - Real-time graphs for pressure, flow, and volume
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <vector>

#ifndef LV_HOR_RES
#define LV_HOR_RES 320
#endif

#ifndef LV_VER_RES
#define LV_VER_RES 240
#endif

// Screen enumeration
enum Screen {
    SCREEN_MAIN,
    SCREEN_SETTINGS,
    SCREEN_ALARMS,
    SCREEN_HISTORY
};

// Ventilation mode enumeration
enum VentilationMode {
    MODE_VCV,  // Volume Controlled Ventilation
    MODE_PCV,  // Pressure Controlled Ventilation
    MODE_VAC   // Volume Assisted Control
};

// Alarm severity enumeration
enum AlarmSeverity {
    ALARM_INFO,
    ALARM_WARNING,
    ALARM_CRITICAL
};

// Data structure for ventilator parameters
struct VentilatorData {
    float pressure;      // Pressure in cmH2O
    float flow;          // Flow in L/min
    float volume;        // Volume in mL
    float fr;            // Respiratory rate in breaths/min
    float peep;          // PEEP in cmH2O
    VentilationMode mode; // Ventilation mode
    bool alarm;          // Alarm state
    float battery;       // Battery level in %
    bool wifi;           // WiFi connection state
};

// Alarm structure
struct Alarm {
    String message;
    String time;
    AlarmSeverity severity;
    bool active;
};

// History event structure
struct HistoryEvent {
    String date;
    String time;
    String event;
};

// Settings structure
struct VentilatorSettings {
    VentilationMode mode;
    float fr;            // Respiratory rate
    float volume;        // Tidal volume
    float peep;          // PEEP
    float ie_ratio;      // I:E ratio
};

class DisplayManager {
private:
    TFT_eSPI tft;
    Screen currentScreen;
    
    // LVGL objects
    lv_obj_t* mainScreen;
    lv_obj_t* settingsScreen;
    lv_obj_t* alarmsScreen;
    lv_obj_t* historyScreen;
    
    // Main screen widgets
    lv_obj_t* headerBar;
    lv_obj_t* statusLED;
    lv_obj_t* metricCards[7];
    lv_obj_t* graphPressure;
    lv_obj_t* graphFlow;
    lv_obj_t* graphVolume;
    lv_chart_series_t* pressureSeries;
    lv_chart_series_t* flowSeries;
    lv_chart_series_t* volumeSeries;
    lv_obj_t* btnSettings;
    lv_obj_t* btnAlarms;
    lv_obj_t* btnHistory;
    
    // Settings screen widgets
    lv_obj_t* dropdownMode;
    lv_obj_t* sliderFR;
    lv_obj_t* sliderVolume;
    lv_obj_t* sliderPEEP;
    lv_obj_t* sliderIERatio;
    lv_obj_t* btnBackSettings;
    lv_obj_t* btnSaveSettings;
    
    // Alarms screen widgets
    lv_obj_t* alarmsList;
    lv_obj_t* btnBackAlarms;
    
    // History screen widgets
    lv_obj_t* historyTable;
    lv_obj_t* btnBackHistory;
    
    // Data storage
    VentilatorData currentData;
    VentilatorSettings currentSettings;
    std::vector<Alarm> alarms;
    std::vector<HistoryEvent> history;
    
    // Graph data buffers
    float pressureBuffer[100];
    float flowBuffer[100];
    float volumeBuffer[100];
    int bufferIndex;
    
    // Private methods
    void createMainScreen();
    void createSettingsScreen();
    void createAlarmsScreen();
    void createHistoryScreen();
    void updateHeaderBar();
    void updateMetricCards();
    void updateGraphs();
    void updateStatusLED();
    void switchScreen(Screen screen);
    static void settingsButtonCallback(lv_event_t* e);
    static void alarmsButtonCallback(lv_event_t* e);
    static void historyButtonCallback(lv_event_t* e);
    static void backButtonCallback(lv_event_t* e);
    static void saveSettingsCallback(lv_event_t* e);
    
public:
    DisplayManager();
    
    /**
     * @brief Initialize the display manager
     */
    void begin();
    
    /**
     * @brief Update the display with new data
     * @param data Ventilator data structure
     */
    void update();
    void update(const VentilatorData& data);
    void setData(const VentilatorData& data);
    
    /**
     * @brief Show alarm on display
     * @param message Alarm message
     * @param severity Alarm severity
     */
    void showAlarm(const String& message, AlarmSeverity severity);
    
    /**
     * @brief Show settings screen
     */
    void showSettings();
    
    /**
     * @brief Show graphs screen
     */
    void showGraphs();
    
    /**
     * @brief Add event to history
     * @param event Event description
     */
    void addHistoryEvent(const String& event);
    
    /**
     * @brief Get current settings
     * @return Current ventilation settings
     */
    VentilatorSettings getSettings() const;
    
    /**
     * @brief Set settings
     * @param settings New ventilation settings
     */
    void setSettings(const VentilatorSettings& settings);
};

#endif // DISPLAY_MANAGER_H
