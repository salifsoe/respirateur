# Respirateur Artificiel - Firmware TFT Interface

## Overview

This firmware provides the TFT display interface for the artificial ventilator system using ESP32, LVGL, and TFT_eSPI. It handles all UI operations including real-time data display, settings management, alarms, and history logging.

**Note:** This firmware only handles the display interface. Motor control, PID algorithms, sensor reading, and ventilation algorithms are implemented separately.

## Hardware Requirements

- ESP32 microcontroller
- TFT Display (ILI9341 recommended, 320x240 resolution)
- SPI connection for TFT
- Optional: Touch screen support

## Software Dependencies

- PlatformIO
- LVGL v8.3.11
- TFT_eSPI v2.5.43
- FreeRTOS (included in ESP32 framework)

## Project Structure

```
firmware/
├── platformio.ini          # PlatformIO configuration
├── include/
│   ├── User_Setup.h        # TFT_eSPI hardware configuration
│   └── lv_conf.h          # LVGL configuration
└── src/
    ├── main.cpp           # Main entry point
    ├── DisplayManager.h    # Display manager header
    └── DisplayManager.cpp # Display manager implementation
```

## Installation

1. Install PlatformIO extension in VS Code or use PlatformIO CLI
2. Open the project directory in PlatformIO
3. Configure TFT pins in `include/User_Setup.h` according to your wiring
4. Build and upload to ESP32:
   ```bash
   pio run --target upload
   ```

## Configuration

### TFT Display Configuration

Edit `include/User_Setup.h` to match your hardware:

```cpp
// Display driver
#define ILI9341_DRIVER  // Uncomment your display driver

// ESP32 pins
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4
```

### LVGL Configuration

Edit `include/lv_conf.h` to adjust LVGL settings:
- Color depth
- Memory size
- Font selections
- Widget usage

## DisplayManager API

The `DisplayManager` class provides the following interface:

### Initialization

```cpp
DisplayManager displayManager;
displayManager.begin();
```

### Update Display

```cpp
// Update with ventilator data
VentilatorData data = {
    .pressure = 22.0,
    .flow = 30.0,
    .volume = 500.0,
    .fr = 15.0,
    .peep = 5.0,
    .mode = MODE_VCV,
    .alarm = false,
    .battery = 94.0,
    .wifi = true
};
displayManager.update(data);
```

### Show Alarm

```cpp
displayManager.showAlarm("High pressure", ALARM_CRITICAL);
```

### Show Settings Screen

```cpp
displayManager.showSettings();
```

### Add History Event

```cpp
displayManager.addHistoryEvent("System started");
```

## Data Structures

### VentilatorData

```cpp
struct VentilatorData {
    float pressure;      // Pressure in cmH2O
    float flow;          // Flow in L/min
    float volume;        // Volume in mL
    float fr;            // Respiratory rate in breaths/min
    float peep;          // PEEP in cmH2O
    VentilationMode mode; // VCV, PCV, or VAC
    bool alarm;          // Alarm state
    float battery;       // Battery level in %
    bool wifi;           // WiFi connection state
};
```

### VentilatorSettings

```cpp
struct VentilatorSettings {
    VentilationMode mode;
    float fr;            // Respiratory rate
    float volume;        // Tidal volume
    float peep;          // PEEP
    float ie_ratio;      // I:E ratio
};
```

## Screens

### Main Screen
- Header with logo, time, WiFi status, battery level
- Status LED (green/orange/red)
- Metric cards: Mode, Pressure, Volume, Flow, FR, PEEP, I:E
- Real-time graphs: Pressure, Flow, Volume
- Navigation buttons: Settings, Alarms, History

### Settings Screen
- Ventilation mode selection (VCV/PCV/VAC)
- Respiratory rate slider (5-40 breaths/min)
- Tidal volume slider (200-800 mL)
- PEEP slider (0-20 cmH2O)
- I:E ratio slider (1:5 to 1:1)
- Save and back buttons

### Alarms Screen
- Alarm list with severity indicators
- Time and message display
- Color-coded by severity (critical/warning/info)

### History Screen
- Event table with date, time, and event description
- Scrollable history log

## Integration with Main Firmware

The main firmware should:

1. Include `DisplayManager.h`
2. Create a `DisplayManager` instance
3. Call `displayManager.begin()` in setup
4. Call `displayManager.update(data)` periodically with sensor data
5. Use `displayManager.showAlarm()` for alarm notifications
6. Use `displayManager.getSettings()` to read user settings
7. Use `displayManager.addHistoryEvent()` to log events

## FreeRTOS Integration

The display runs in a separate FreeRTOS task with 50ms refresh rate:

```cpp
xTaskCreate(
    displayTask,
    "DisplayTask",
    8192,
    NULL,
    2,
    &displayTaskHandle
);
```

## MQTT Integration

Settings changes from the TFT interface should be published via MQTT to the `ventilator/config` topic. This integration is handled by the main firmware.

## Troubleshooting

### Display Not Showing
- Check SPI wiring
- Verify pin configuration in `User_Setup.h`
- Ensure display driver is correct

### LVGL Memory Issues
- Increase `LV_MEM_SIZE` in `lv_conf.h`
- Reduce font selections
- Disable unused widgets

### Touch Not Working
- Configure touch pins in `User_Setup.h`
- Implement touch callback in `DisplayManager.cpp`

## License

This project is part of an academic project for demonstration purposes.
