# Respirateur Artificiel Automatisé

## ⚠️ Note Importante

**Ce projet est une SIMULATION LOGICIELLE** du système de respirateur artificiel. Il ne constitue pas un prototype physique fonctionnel et ne doit en aucun cas être utilisé sur un patient réel.

Pour plus d'informations, consultez [NOTE_IMPORTANTE.md](NOTE_IMPORTANTE.md).

## Overview

This project is a software simulation of an automated artificial ventilator system with a professional-grade interface for medical monitoring. It consists of two main components:

1. **ESP32 TFT Interface** - On-device display using LVGL with simulated data
2. **Web Dashboard** - Remote monitoring interface using React with simulated data

**Important:** This project is a simulation. The physical prototype described in the mémoire includes motor control, PID algorithms, real sensors, and ventilation algorithms that are not implemented in this simulation.

## Project Structure

```
Respirateur/
├── NOTE_IMPORTANTE.md     # Documentation sur la nature du projet
├── GUIDE_TRANSITION.md    # Guide vers prototype physique
├── firmware/              # ESP32 TFT interface (simulation)
│   ├── src/
│   │   ├── main.cpp
│   │   ├── DisplayManager.h
│   │   └── DisplayManager.cpp
│   ├── include/
│   │   ├── User_Setup.h
│   │   └── lv_conf.h
│   ├── platformio.ini
│   ├── GUIDE_ELECTROMECANICIEN.md
│   └── README.md
└── dashboard/            # Web dashboard (simulation)
    ├── src/
    │   ├── components/
    │   ├── layouts/
    │   ├── pages/
    │   ├── hooks/
    │   ├── services/
    │   └── types/
    ├── tailwind.config.js
    ├── package.json
    └── README.md
```

## Features

### ESP32 TFT Interface
- **Main Screen:** Real-time display of pressure, flow, volume, FR, PEEP, mode, and I:E ratio
- **Real-time Graphs:** Live waveforms for pressure, flow, and volume
- **Settings Screen:** Adjustable ventilation parameters (mode, FR, volume, PEEP, I:E)
- **Alarms Screen:** Alarm history with severity indicators
- **History Screen:** Event log with timestamps
- **Status Indicators:** WiFi, battery, and system status LED
- **Modern UI:** Inspired by professional medical ventilators (Dräger, Hamilton, Getinge)

### Web Dashboard
- **Dashboard Page:** Real-time metrics with status indicators
- **Live Graphs:** 100-point buffer for smooth real-time visualization
- **Alarms Page:** Searchable and filterable alarm history
- **Settings Page:** Remote parameter configuration
- **Mock Mode:** Testing without hardware
- **MQTT Integration:** Real-time communication with ESP32
- **Responsive Design:** Works on desktop and tablet

## Technology Stack

### Firmware
- **PlatformIO** - Build system
- **ESP32** - Microcontroller
- **LVGL v8.3.11** - Graphics library
- **TFT_eSPI v2.5.43** - Display driver
- **FreeRTOS** - Real-time operating system

### Web Dashboard
- **React 18** - UI framework
- **TypeScript** - Type safety
- **Vite** - Build tool
- **TailwindCSS** - Styling
- **React Router** - Navigation
- **Recharts** - Data visualization
- **MQTT.js** - Communication protocol
- **React Icons** - Icon library

## Quick Start

### Firmware (ESP32)

1. Install PlatformIO in VS Code
2. Navigate to `firmware/` directory
3. Configure TFT pins in `include/User_Setup.h`
4. Build and upload:
   ```bash
   pio run --target upload
   ```

See [firmware/README.md](firmware/README.md) for detailed instructions.

### Web Dashboard

1. Navigate to `dashboard/` directory
2. Install dependencies:
   ```bash
   npm install
   ```
3. Start development server:
   ```bash
   npm run dev
   ```

See [dashboard/README.md](dashboard/README.md) for detailed instructions.

## MQTT Communication

### Topics

**ESP32 → Dashboard:**
- `ventilator/data` - Real-time ventilator parameters

**Dashboard → ESP32:**
- `ventilator/config` - Ventilation settings

### Data Format

**Ventilator Data:**
```json
{
  "pressure": 22,
  "flow": 30,
  "volume": 500,
  "fr": 15,
  "peep": 5,
  "mode": "VCV",
  "alarm": false,
  "battery": 94,
  "wifi": true
}
```

**Ventilator Config:**
```json
{
  "mode": "VCV",
  "fr": 15,
  "volume": 500,
  "peep": 5,
  "ie_ratio": 0.5
}
```

## Ventilation Modes

- **VCV** - Volume Controlled Ventilation
- **PCV** - Pressure Controlled Ventilation
- **VAC** - Volume Assisted Control

## Color Scheme

### Medical-Grade Palette
- **Background:** Very dark gray (`#0a0a0f`)
- **Cards:** Night blue (`#1a1a2e`)
- **Primary:** Cyan (`#00d4ff`)
- **Success:** Green (`#00ff00`)
- **Warning:** Orange (`#ffaa00`)
- **Danger:** Red (`#ff0000`)

## Integration with Main Firmware

The main firmware (motor control, PID, sensors) should:

1. Include `DisplayManager.h` for TFT interface
2. Call `displayManager.update(data)` with sensor data
3. Use `displayManager.getSettings()` to read user settings
4. Publish data to MQTT topic `ventilator/data`
5. Subscribe to `ventilator/config` for remote settings

## Testing

### Web Dashboard Mock Mode

The dashboard includes a mock MQTT service for testing without ESP32:

1. Start the dashboard: `npm run dev`
2. Dashboard automatically starts in mock mode
3. Realistic data is generated every 100ms
4. Toggle between mock and real MQTT using the header button

### Firmware Testing

Use the DisplayManager API with simulated data:

```cpp
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

## Architecture

### SOLID Principles

- **Single Responsibility:** Each component has one clear purpose
- **Open/Closed:** Components are extensible without modification
- **Liskov Substitution:** Interfaces are properly implemented
- **Interface Segregation:** Focused, minimal interfaces
- **Dependency Inversion:** Depend on abstractions, not concretions

### Code Quality

- **TypeScript:** Type safety in web dashboard
- **Modular Design:** Reusable components and services
- **Documentation:** Comprehensive comments and README files
- **Clean Code:** Follows best practices and conventions

## Hardware Requirements

### ESP32 TFT Interface
- ESP32 microcontroller
- TFT Display (ILI9341 recommended, 320x240)
- SPI connection for TFT
- Optional: Touch screen support

### Web Dashboard
- Modern web browser (Chrome, Firefox, Safari, Edge)
- MQTT broker (for real mode)
- Network connection

## Future Enhancements

- [ ] Touch screen support for TFT
- [ ] Patient data logging
- [ ] Multiple ventilator support
- [ ] User authentication
- [ ] Data export functionality
- [ ] Mobile app version
- [ ] Cloud integration
- [ ] Advanced analytics

## Troubleshooting

### Firmware Issues
- Check TFT pin configuration in `User_Setup.h`
- Verify SPI wiring
- Increase LVGL memory if needed
- See [firmware/README.md](firmware/README.md) for details

### Dashboard Issues
- Verify MQTT broker URL
- Check browser console for errors
- Ensure TailwindCSS is configured
- See [dashboard/README.md](dashboard/README.md) for details

## Contributing

This is an academic project for demonstration purposes. When integrating with the main firmware:

1. Follow the DisplayManager API
2. Use the defined data structures
3. Publish to the correct MQTT topics
4. Maintain the separation of concerns

## License

This project is part of an academic project for demonstration purposes.

## Team

- **TFT Interface:** DisplayManager, LVGL UI, Screens
- **Web Dashboard:** React components, MQTT integration, Visualization
- **Main Firmware:** Motor control, PID, sensors, ventilation algorithms (separate developer)

## Acknowledgments

- LVGL - Lightweight graphics library
- TFT_eSPI - TFT display driver
- React - UI framework
- Recharts - Data visualization
- MQTT.js - MQTT client library

---

**Note:** This project is designed for educational and demonstration purposes. It is not intended for actual medical use without proper certification and clinical validation.
