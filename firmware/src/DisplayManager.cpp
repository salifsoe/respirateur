/**
 * @file DisplayManager.cpp
 * @brief Implementation of DisplayManager class
 */

#include "DisplayManager.h"
#include <vector>
#include <string>

// LVGL display buffer
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[LV_HOR_RES * 10];
static TFT_eSPI* activeTft = nullptr;

// LVGL flush callback
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    if (activeTft == nullptr) {
        lv_disp_flush_ready(disp);
        return;
    }

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    activeTft->startWrite();
    activeTft->setAddrWindow(area->x1, area->y1, w, h);
    activeTft->pushColors((uint16_t *)&color_p->full, w * h, true);
    activeTft->endWrite();

    lv_disp_flush_ready(disp);
}

// LVGL touch callback (if touch is available)
void my_touchpad_read(lv_indev_drv_t *indev, lv_indev_data_t *data) {
    // Touch implementation would go here
    // For now, we'll use buttons or simulated input
    data->state = LV_INDEV_STATE_REL;
}

DisplayManager::DisplayManager() 
    : currentScreen(SCREEN_MAIN),
      pressureSeries(nullptr),
      flowSeries(nullptr),
      volumeSeries(nullptr),
      bufferIndex(0) {
    
    // Initialize default data
    currentData = {
        .pressure = 0.0f,
        .flow = 0.0f,
        .volume = 0.0f,
        .fr = 15.0f,
        .peep = 5.0f,
        .mode = MODE_VCV,
        .alarm = false,
        .battery = 100.0f,
        .wifi = false
    };
    
    // Initialize default settings
    currentSettings = {
        .mode = MODE_VCV,
        .fr = 15.0f,
        .volume = 500.0f,
        .peep = 5.0f,
        .ie_ratio = 1.0f / 2.0f
    };
    
    // Initialize buffers
    for (int i = 0; i < 100; i++) {
        pressureBuffer[i] = 0.0f;
        flowBuffer[i] = 0.0f;
        volumeBuffer[i] = 0.0f;
    }
}

void DisplayManager::begin() {
    Serial.println("Initializing Display Manager...");
    
    // Initialize TFT
    activeTft = &tft;
    tft.begin();
    tft.setRotation(1);
    
    // Initialize LVGL
    lv_init();
    
    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, LV_HOR_RES * 10);
    
    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES;
    disp_drv.ver_res = LV_VER_RES;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    
    // Initialize input device (touch or buttons)
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
    
    // Create screens
    createMainScreen();
    createSettingsScreen();
    createAlarmsScreen();
    createHistoryScreen();
    
    // Load main screen
    lv_scr_load(mainScreen);
    
    Serial.println("Display Manager initialized successfully");
}

void DisplayManager::createMainScreen() {
    mainScreen = lv_obj_create(NULL);
    
    // Header bar
    headerBar = lv_obj_create(mainScreen);
    lv_obj_set_size(headerBar, LV_HOR_RES, 40);
    lv_obj_set_pos(headerBar, 0, 0);
    lv_obj_set_style_bg_color(headerBar, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_border_width(headerBar, 0, 0);
    
    // Logo label
    lv_obj_t* logoLabel = lv_label_create(headerBar);
    lv_label_set_text(logoLabel, "MED-VENT");
    lv_obj_set_style_text_color(logoLabel, lv_color_hex(0x00d4ff), 0);
    lv_obj_set_style_text_font(logoLabel, &lv_font_montserrat_16, 0);
    lv_obj_align(logoLabel, LV_ALIGN_LEFT_MID, 10, 0);
    
    // Time label
    lv_obj_t* timeLabel = lv_label_create(headerBar);
    lv_label_set_text(timeLabel, "00:00");
    lv_obj_set_style_text_color(timeLabel, lv_color_white(), 0);
    lv_obj_align(timeLabel, LV_ALIGN_CENTER, 0, 0);
    
    // WiFi status
    lv_obj_t* wifiLabel = lv_label_create(headerBar);
    lv_label_set_text(wifiLabel, LV_SYMBOL_WIFI);
    lv_obj_set_style_text_color(wifiLabel, lv_color_hex(0x888888), 0);
    lv_obj_align(wifiLabel, LV_ALIGN_RIGHT_MID, -50, 0);
    
    // Battery status
    lv_obj_t* batteryLabel = lv_label_create(headerBar);
    lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_FULL);
    lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0x00ff00), 0);
    lv_obj_align(batteryLabel, LV_ALIGN_RIGHT_MID, -20, 0);
    
    // Status LED
    statusLED = lv_obj_create(headerBar);
    lv_obj_set_size(statusLED, 20, 20);
    lv_obj_set_style_bg_color(statusLED, lv_color_hex(0x00ff00), 0);
    lv_obj_set_style_radius(statusLED, 10, 0);
    lv_obj_align(statusLED, LV_ALIGN_RIGHT_MID, -10, 0);
    
    // Metric cards container
    lv_obj_t* cardsContainer = lv_obj_create(mainScreen);
    lv_obj_set_size(cardsContainer, LV_HOR_RES - 20, 120);
    lv_obj_set_pos(cardsContainer, 10, 50);
    lv_obj_set_style_bg_color(cardsContainer, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_width(cardsContainer, 0, 0);
    lv_obj_set_style_radius(cardsContainer, 10, 0);
    
    // Create metric cards
    const char* cardLabels[] = {"Mode", "Press", "Vol", "Flow", "FR", "PEEP", "I:E"};
    const char* cardUnits[] = {"", "cmH2O", "mL", "L/min", "/min", "cmH2O", ""};
    
    for (int i = 0; i < 7; i++) {
        metricCards[i] = lv_obj_create(cardsContainer);
        lv_obj_set_size(metricCards[i], (LV_HOR_RES - 30) / 7, 100);
        lv_obj_set_pos(metricCards[i], i * ((LV_HOR_RES - 30) / 7) + 5, 10);
        lv_obj_set_style_bg_color(metricCards[i], lv_color_hex(0x0f3460), 0);
        lv_obj_set_style_border_width(metricCards[i], 0, 0);
        lv_obj_set_style_radius(metricCards[i], 8, 0);
        
        lv_obj_t* label = lv_label_create(metricCards[i]);
        lv_label_set_text(label, cardLabels[i]);
        lv_obj_set_style_text_color(label, lv_color_hex(0x888888), 0);
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 5);
        
        lv_obj_t* value = lv_label_create(metricCards[i]);
        lv_label_set_text(value, "--");
        lv_obj_set_style_text_color(value, lv_color_white(), 0);
        lv_obj_align(value, LV_ALIGN_CENTER, 0, 0);
        
        lv_obj_t* unit = lv_label_create(metricCards[i]);
        lv_label_set_text(unit, cardUnits[i]);
        lv_obj_set_style_text_color(unit, lv_color_hex(0x00d4ff), 0);
        lv_obj_align(unit, LV_ALIGN_BOTTOM_MID, 0, -5);
    }
    
    // Graphs container
    lv_obj_t* graphsContainer = lv_obj_create(mainScreen);
    lv_obj_set_size(graphsContainer, LV_HOR_RES - 20, 100);
    lv_obj_set_pos(graphsContainer, 10, 180);
    lv_obj_set_style_bg_color(graphsContainer, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_width(graphsContainer, 0, 0);
    lv_obj_set_style_radius(graphsContainer, 10, 0);
    
    // Create pressure graph
    graphPressure = lv_chart_create(graphsContainer);
    lv_obj_set_size(graphPressure, (LV_HOR_RES - 40) / 3, 80);
    lv_obj_set_pos(graphPressure, 5, 10);
    lv_chart_set_type(graphPressure, LV_CHART_TYPE_LINE);
    lv_chart_set_range(graphPressure, LV_CHART_AXIS_PRIMARY_Y, 0, 40);
    lv_chart_set_point_count(graphPressure, 50);
    lv_chart_set_update_mode(graphPressure, LV_CHART_UPDATE_MODE_SHIFT);
    pressureSeries = lv_chart_add_series(graphPressure, lv_color_hex(0x00d4ff), LV_CHART_AXIS_PRIMARY_Y);
    
    lv_obj_t* pressureLabel = lv_label_create(graphsContainer);
    lv_label_set_text(pressureLabel, "Pressure");
    lv_obj_set_style_text_color(pressureLabel, lv_color_hex(0x00d4ff), 0);
    lv_obj_align(pressureLabel, LV_ALIGN_TOP_LEFT, 10, -5);
    
    // Create flow graph
    graphFlow = lv_chart_create(graphsContainer);
    lv_obj_set_size(graphFlow, (LV_HOR_RES - 40) / 3, 80);
    lv_obj_set_pos(graphFlow, (LV_HOR_RES - 40) / 3 + 15, 10);
    lv_chart_set_type(graphFlow, LV_CHART_TYPE_LINE);
    lv_chart_set_range(graphFlow, LV_CHART_AXIS_PRIMARY_Y, -60, 60);
    lv_chart_set_point_count(graphFlow, 50);
    lv_chart_set_update_mode(graphFlow, LV_CHART_UPDATE_MODE_SHIFT);
    flowSeries = lv_chart_add_series(graphFlow, lv_color_hex(0x00ff00), LV_CHART_AXIS_PRIMARY_Y);
    
    lv_obj_t* flowLabel = lv_label_create(graphsContainer);
    lv_label_set_text(flowLabel, "Flow");
    lv_obj_set_style_text_color(flowLabel, lv_color_hex(0x00ff00), 0);
    lv_obj_align(flowLabel, LV_ALIGN_TOP_MID, 0, -5);
    
    // Create volume graph
    graphVolume = lv_chart_create(graphsContainer);
    lv_obj_set_size(graphVolume, (LV_HOR_RES - 40) / 3, 80);
    lv_obj_set_pos(graphVolume, 2 * ((LV_HOR_RES - 40) / 3) + 25, 10);
    lv_chart_set_type(graphVolume, LV_CHART_TYPE_LINE);
    lv_chart_set_range(graphVolume, LV_CHART_AXIS_PRIMARY_Y, 0, 800);
    lv_chart_set_point_count(graphVolume, 50);
    lv_chart_set_update_mode(graphVolume, LV_CHART_UPDATE_MODE_SHIFT);
    volumeSeries = lv_chart_add_series(graphVolume, lv_color_hex(0xffaa00), LV_CHART_AXIS_PRIMARY_Y);
    
    lv_obj_t* volumeLabel = lv_label_create(graphsContainer);
    lv_label_set_text(volumeLabel, "Volume");
    lv_obj_set_style_text_color(volumeLabel, lv_color_hex(0xffaa00), 0);
    lv_obj_align(volumeLabel, LV_ALIGN_TOP_RIGHT, -10, -5);
    
    // Bottom buttons
    btnSettings = lv_btn_create(mainScreen);
    lv_obj_set_size(btnSettings, 80, 40);
    lv_obj_set_pos(btnSettings, 20, LV_VER_RES - 50);
    lv_obj_set_style_bg_color(btnSettings, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnSettings, settingsButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* settingsLabel = lv_label_create(btnSettings);
    lv_label_set_text(settingsLabel, LV_SYMBOL_SETTINGS " Param");
    lv_obj_center(settingsLabel);
    
    btnAlarms = lv_btn_create(mainScreen);
    lv_obj_set_size(btnAlarms, 80, 40);
    lv_obj_set_pos(btnAlarms, LV_HOR_RES / 2 - 40, LV_VER_RES - 50);
    lv_obj_set_style_bg_color(btnAlarms, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnAlarms, alarmsButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* alarmsLabel = lv_label_create(btnAlarms);
    lv_label_set_text(alarmsLabel, LV_SYMBOL_BELL " Alarm");
    lv_obj_center(alarmsLabel);
    
    btnHistory = lv_btn_create(mainScreen);
    lv_obj_set_size(btnHistory, 80, 40);
    lv_obj_set_pos(btnHistory, LV_HOR_RES - 100, LV_VER_RES - 50);
    lv_obj_set_style_bg_color(btnHistory, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnHistory, historyButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* historyLabel = lv_label_create(btnHistory);
    lv_label_set_text(historyLabel, LV_SYMBOL_LIST " Hist");
    lv_obj_center(historyLabel);
}

void DisplayManager::createSettingsScreen() {
    settingsScreen = lv_obj_create(NULL);
    
    // Header
    lv_obj_t* header = lv_obj_create(settingsScreen);
    lv_obj_set_size(header, LV_HOR_RES, 40);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_label_set_text(titleLabel, "Settings");
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);
    
    // Back button
    btnBackSettings = lv_btn_create(header);
    lv_obj_set_size(btnBackSettings, 60, 30);
    lv_obj_set_pos(btnBackSettings, 10, 5);
    lv_obj_set_style_bg_color(btnBackSettings, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnBackSettings, backButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* backLabel = lv_label_create(btnBackSettings);
    lv_label_set_text(backLabel, LV_SYMBOL_LEFT);
    lv_obj_center(backLabel);
    
    // Settings container
    lv_obj_t* settingsContainer = lv_obj_create(settingsScreen);
    lv_obj_set_size(settingsContainer, LV_HOR_RES - 40, LV_VER_RES - 100);
    lv_obj_set_pos(settingsContainer, 20, 50);
    lv_obj_set_style_bg_color(settingsContainer, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_width(settingsContainer, 0, 0);
    lv_obj_set_style_radius(settingsContainer, 10, 0);
    
    // Mode dropdown
    lv_obj_t* modeLabel = lv_label_create(settingsContainer);
    lv_label_set_text(modeLabel, "Ventilation Mode");
    lv_obj_set_style_text_color(modeLabel, lv_color_white(), 0);
    lv_obj_set_pos(modeLabel, 10, 10);
    
    dropdownMode = lv_dropdown_create(settingsContainer);
    lv_dropdown_set_options(dropdownMode, "VCV\nPCV\nVAC");
    lv_obj_set_pos(dropdownMode, 10, 35);
    lv_obj_set_width(dropdownMode, LV_HOR_RES - 60);
    
    // FR slider
    lv_obj_t* frLabel = lv_label_create(settingsContainer);
    lv_label_set_text(frLabel, "Respiratory Rate (breaths/min)");
    lv_obj_set_style_text_color(frLabel, lv_color_white(), 0);
    lv_obj_set_pos(frLabel, 10, 70);
    
    sliderFR = lv_slider_create(settingsContainer);
    lv_slider_set_range(sliderFR, 5, 40);
    lv_slider_set_value(sliderFR, 15, LV_ANIM_OFF);
    lv_obj_set_pos(sliderFR, 10, 95);
    lv_obj_set_width(sliderFR, LV_HOR_RES - 60);
    
    // Volume slider
    lv_obj_t* volumeLabel = lv_label_create(settingsContainer);
    lv_label_set_text(volumeLabel, "Tidal Volume (mL)");
    lv_obj_set_style_text_color(volumeLabel, lv_color_white(), 0);
    lv_obj_set_pos(volumeLabel, 10, 130);
    
    sliderVolume = lv_slider_create(settingsContainer);
    lv_slider_set_range(sliderVolume, 200, 800);
    lv_slider_set_value(sliderVolume, 500, LV_ANIM_OFF);
    lv_obj_set_pos(sliderVolume, 10, 155);
    lv_obj_set_width(sliderVolume, LV_HOR_RES - 60);
    
    // PEEP slider
    lv_obj_t* peepLabel = lv_label_create(settingsContainer);
    lv_label_set_text(peepLabel, "PEEP (cmH2O)");
    lv_obj_set_style_text_color(peepLabel, lv_color_white(), 0);
    lv_obj_set_pos(peepLabel, 10, 190);
    
    sliderPEEP = lv_slider_create(settingsContainer);
    lv_slider_set_range(sliderPEEP, 0, 20);
    lv_slider_set_value(sliderPEEP, 5, LV_ANIM_OFF);
    lv_obj_set_pos(sliderPEEP, 10, 215);
    lv_obj_set_width(sliderPEEP, LV_HOR_RES - 60);
    
    // I:E ratio slider
    lv_obj_t* ieLabel = lv_label_create(settingsContainer);
    lv_label_set_text(ieLabel, "I:E Ratio");
    lv_obj_set_style_text_color(ieLabel, lv_color_white(), 0);
    lv_obj_set_pos(ieLabel, 10, 250);
    
    sliderIERatio = lv_slider_create(settingsContainer);
    lv_slider_set_range(sliderIERatio, 10, 50);
    lv_slider_set_value(sliderIERatio, 33, LV_ANIM_OFF);
    lv_obj_set_pos(sliderIERatio, 10, 275);
    lv_obj_set_width(sliderIERatio, LV_HOR_RES - 60);
    
    // Save button
    btnSaveSettings = lv_btn_create(settingsScreen);
    lv_obj_set_size(btnSaveSettings, 100, 40);
    lv_obj_set_pos(btnSaveSettings, LV_HOR_RES / 2 - 50, LV_VER_RES - 50);
    lv_obj_set_style_bg_color(btnSaveSettings, lv_color_hex(0x00d4ff), 0);
    lv_obj_add_event_cb(btnSaveSettings, saveSettingsCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* saveLabel = lv_label_create(btnSaveSettings);
    lv_label_set_text(saveLabel, "Save");
    lv_obj_center(saveLabel);
}

void DisplayManager::createAlarmsScreen() {
    alarmsScreen = lv_obj_create(NULL);
    
    // Header
    lv_obj_t* header = lv_obj_create(alarmsScreen);
    lv_obj_set_size(header, LV_HOR_RES, 40);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_label_set_text(titleLabel, "Alarms");
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);
    
    // Back button
    btnBackAlarms = lv_btn_create(header);
    lv_obj_set_size(btnBackAlarms, 60, 30);
    lv_obj_set_pos(btnBackAlarms, 10, 5);
    lv_obj_set_style_bg_color(btnBackAlarms, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnBackAlarms, backButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* backLabel = lv_label_create(btnBackAlarms);
    lv_label_set_text(backLabel, LV_SYMBOL_LEFT);
    lv_obj_center(backLabel);
    
    // Alarms list
    alarmsList = lv_list_create(alarmsScreen);
    lv_obj_set_size(alarmsList, LV_HOR_RES - 20, LV_VER_RES - 60);
    lv_obj_set_pos(alarmsList, 10, 50);
    lv_obj_set_style_bg_color(alarmsList, lv_color_hex(0x16213e), 0);
}

void DisplayManager::createHistoryScreen() {
    historyScreen = lv_obj_create(NULL);
    
    // Header
    lv_obj_t* header = lv_obj_create(historyScreen);
    lv_obj_set_size(header, LV_HOR_RES, 40);
    lv_obj_set_style_bg_color(header, lv_color_hex(0x1a1a2e), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    
    lv_obj_t* titleLabel = lv_label_create(header);
    lv_label_set_text(titleLabel, "History");
    lv_obj_set_style_text_color(titleLabel, lv_color_white(), 0);
    lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, 0);
    
    // Back button
    btnBackHistory = lv_btn_create(header);
    lv_obj_set_size(btnBackHistory, 60, 30);
    lv_obj_set_pos(btnBackHistory, 10, 5);
    lv_obj_set_style_bg_color(btnBackHistory, lv_color_hex(0x0f3460), 0);
    lv_obj_add_event_cb(btnBackHistory, backButtonCallback, LV_EVENT_CLICKED, this);
    
    lv_obj_t* backLabel = lv_label_create(btnBackHistory);
    lv_label_set_text(backLabel, LV_SYMBOL_LEFT);
    lv_obj_center(backLabel);
    
    // History table
    historyTable = lv_table_create(historyScreen);
    lv_obj_set_size(historyTable, LV_HOR_RES - 20, LV_VER_RES - 60);
    lv_obj_set_pos(historyTable, 10, 50);
    lv_obj_set_style_bg_color(historyTable, lv_color_hex(0x16213e), 0);
    
    lv_table_set_cell_value(historyTable, 0, 0, "Date");
    lv_table_set_cell_value(historyTable, 0, 1, "Time");
    lv_table_set_cell_value(historyTable, 0, 2, "Event");
    
    lv_obj_set_style_text_color(historyTable, lv_color_white(), 0);
}

void DisplayManager::update() {
    // Update header bar
    updateHeaderBar();
    
    // Update metric cards
    updateMetricCards();
    
    // Update graphs
    updateGraphs();
    
    // Update status LED
    updateStatusLED();
    
    // LVGL timer handler
    lv_timer_handler();
}

void DisplayManager::update(const VentilatorData& data) {
    currentData = data;
    update();
}

void DisplayManager::setData(const VentilatorData& data) {
    currentData = data;
}

void DisplayManager::updateHeaderBar() {
    // Update WiFi status
    lv_obj_t* wifiLabel = lv_obj_get_child(headerBar, 2);
    lv_obj_set_style_text_color(wifiLabel, 
        currentData.wifi ? lv_color_hex(0x00ff00) : lv_color_hex(0x888888), 0);
    
    // Update battery status
    lv_obj_t* batteryLabel = lv_obj_get_child(headerBar, 3);
    if (currentData.battery > 50) {
        lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_FULL);
        lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0x00ff00), 0);
    } else if (currentData.battery > 20) {
        lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_2);
        lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0xffaa00), 0);
    } else {
        lv_label_set_text(batteryLabel, LV_SYMBOL_BATTERY_EMPTY);
        lv_obj_set_style_text_color(batteryLabel, lv_color_hex(0xff0000), 0);
    }
}

void DisplayManager::updateMetricCards() {
    // Update mode card
    lv_obj_t* modeCard = metricCards[0];
    lv_obj_t* modeValue = lv_obj_get_child(modeCard, 1);
    const char* modeStr = currentData.mode == MODE_VCV ? "VCV" : 
                          currentData.mode == MODE_PCV ? "PCV" : "VAC";
    lv_label_set_text(modeValue, modeStr);
    
    // Update pressure card
    lv_obj_t* pressureCard = metricCards[1];
    lv_obj_t* pressureValue = lv_obj_get_child(pressureCard, 1);
    char pressureStr[16];
    snprintf(pressureStr, sizeof(pressureStr), "%.1f", currentData.pressure);
    lv_label_set_text(pressureValue, pressureStr);
    
    // Update volume card
    lv_obj_t* volumeCard = metricCards[2];
    lv_obj_t* volumeValue = lv_obj_get_child(volumeCard, 1);
    char volumeStr[16];
    snprintf(volumeStr, sizeof(volumeStr), "%.0f", currentData.volume);
    lv_label_set_text(volumeValue, volumeStr);
    
    // Update flow card
    lv_obj_t* flowCard = metricCards[3];
    lv_obj_t* flowValue = lv_obj_get_child(flowCard, 1);
    char flowStr[16];
    snprintf(flowStr, sizeof(flowStr), "%.1f", currentData.flow);
    lv_label_set_text(flowValue, flowStr);
    
    // Update FR card
    lv_obj_t* frCard = metricCards[4];
    lv_obj_t* frValue = lv_obj_get_child(frCard, 1);
    char frStr[16];
    snprintf(frStr, sizeof(frStr), "%.0f", currentData.fr);
    lv_label_set_text(frValue, frStr);
    
    // Update PEEP card
    lv_obj_t* peepCard = metricCards[5];
    lv_obj_t* peepValue = lv_obj_get_child(peepCard, 1);
    char peepStr[16];
    snprintf(peepStr, sizeof(peepStr), "%.1f", currentData.peep);
    lv_label_set_text(peepValue, peepStr);
    
    // Update I:E card
    lv_obj_t* ieCard = metricCards[6];
    lv_obj_t* ieValue = lv_obj_get_child(ieCard, 1);
    float ieRatio = 1.0f / 2.0f; // Default
    char ieStr[16];
    snprintf(ieStr, sizeof(ieStr), "1:%.1f", ieRatio);
    lv_label_set_text(ieValue, ieStr);
}

void DisplayManager::updateGraphs() {
    if (pressureSeries == nullptr || flowSeries == nullptr || volumeSeries == nullptr) {
        return;
    }

    // Update pressure graph
    lv_chart_set_next_value(graphPressure, pressureSeries, static_cast<lv_coord_t>(currentData.pressure));
    
    // Update flow graph
    lv_chart_set_next_value(graphFlow, flowSeries, static_cast<lv_coord_t>(currentData.flow));
    
    // Update volume graph
    lv_chart_set_next_value(graphVolume, volumeSeries, static_cast<lv_coord_t>(currentData.volume));
}

void DisplayManager::updateStatusLED() {
    if (currentData.alarm) {
        lv_obj_set_style_bg_color(statusLED, lv_color_hex(0xff0000), 0);
    } else {
        lv_obj_set_style_bg_color(statusLED, lv_color_hex(0x00ff00), 0);
    }
}

void DisplayManager::switchScreen(Screen screen) {
    currentScreen = screen;
    
    switch (screen) {
        case SCREEN_MAIN:
            lv_scr_load(mainScreen);
            break;
        case SCREEN_SETTINGS:
            lv_scr_load(settingsScreen);
            break;
        case SCREEN_ALARMS:
            lv_scr_load(alarmsScreen);
            break;
        case SCREEN_HISTORY:
            lv_scr_load(historyScreen);
            break;
    }
}

void DisplayManager::showAlarm(const String& message, AlarmSeverity severity) {
    Alarm newAlarm = {message, "", severity, true};
    alarms.push_back(newAlarm);
    
    // Add to alarms list
    lv_obj_t* btn = lv_list_add_btn(alarmsList, 
        severity == ALARM_CRITICAL ? LV_SYMBOL_WARNING : 
        severity == ALARM_WARNING ? LV_SYMBOL_BELL : LV_SYMBOL_INFO, 
        message.c_str());
    
    lv_obj_set_style_text_color(btn, 
        severity == ALARM_CRITICAL ? lv_color_hex(0xff0000) : 
        severity == ALARM_WARNING ? lv_color_hex(0xffaa00) : lv_color_hex(0x00ff00), 0);
}

void DisplayManager::showSettings() {
    switchScreen(SCREEN_SETTINGS);
}

void DisplayManager::showGraphs() {
    switchScreen(SCREEN_MAIN);
}

void DisplayManager::addHistoryEvent(const String& event) {
    HistoryEvent newEvent;
    // Get current time (simplified)
    newEvent.date = "2024-01-01";
    newEvent.time = "12:00";
    newEvent.event = event;
    
    history.push_back(newEvent);
    
    // Add to history table
    int row = history.size();
    lv_table_set_cell_value(historyTable, row, 0, newEvent.date.c_str());
    lv_table_set_cell_value(historyTable, row, 1, newEvent.time.c_str());
    lv_table_set_cell_value(historyTable, row, 2, newEvent.event.c_str());
}

VentilatorSettings DisplayManager::getSettings() const {
    return currentSettings;
}

void DisplayManager::setSettings(const VentilatorSettings& settings) {
    currentSettings = settings;
}

// Callback functions
void DisplayManager::settingsButtonCallback(lv_event_t* e) {
    DisplayManager* dm = (DisplayManager*)lv_event_get_user_data(e);
    dm->showSettings();
}

void DisplayManager::alarmsButtonCallback(lv_event_t* e) {
    DisplayManager* dm = (DisplayManager*)lv_event_get_user_data(e);
    dm->switchScreen(SCREEN_ALARMS);
}

void DisplayManager::historyButtonCallback(lv_event_t* e) {
    DisplayManager* dm = (DisplayManager*)lv_event_get_user_data(e);
    dm->switchScreen(SCREEN_HISTORY);
}

void DisplayManager::backButtonCallback(lv_event_t* e) {
    DisplayManager* dm = (DisplayManager*)lv_event_get_user_data(e);
    dm->switchScreen(SCREEN_MAIN);
}

void DisplayManager::saveSettingsCallback(lv_event_t* e) {
    DisplayManager* dm = (DisplayManager*)lv_event_get_user_data(e);
    
    // Read values from UI
    int modeIndex = lv_dropdown_get_selected(dm->dropdownMode);
    dm->currentSettings.mode = (VentilationMode)modeIndex;
    dm->currentSettings.fr = lv_slider_get_value(dm->sliderFR);
    dm->currentSettings.volume = lv_slider_get_value(dm->sliderVolume);
    dm->currentSettings.peep = lv_slider_get_value(dm->sliderPEEP);
    dm->currentSettings.ie_ratio = lv_slider_get_value(dm->sliderIERatio) / 100.0f;
    
    // Add to history
    dm->addHistoryEvent("Settings updated");
    
    // Return to main screen
    dm->switchScreen(SCREEN_MAIN);
}
