# Respirateur Artificiel - Web Dashboard

## ⚠️ Note Importante

**Ce dashboard est une SIMULATION** du système de surveillance de respirateur artificiel. Les données affichées sont simulées et ne proviennent pas de capteurs réels. Ce dashboard ne doit en aucun cas être utilisé pour surveiller un patient réel.

Pour plus d'informations, consultez [../NOTE_IMPORTANTE.md](../NOTE_IMPORTANTE.md).

## Overview

This is the web dashboard for the artificial ventilator monitoring system. It provides real-time visualization of ventilator parameters, alarms, and settings through a modern React-based interface.

**Note:** This dashboard is a simulation. The physical prototype described in the mémoire includes real sensors, PID control, and motor control that are not implemented in this simulation.

## Tech Stack

- **React 18** - UI framework
- **TypeScript** - Type safety
- **Vite** - Build tool and dev server
- **TailwindCSS** - Styling
- **React Router** - Navigation
- **Recharts** - Real-time data visualization
- **MQTT.js** - MQTT communication
- **React Icons** - Icon library

## Project Structure

```
dashboard/
├── src/
│   ├── components/       # Reusable components
│   │   ├── MetricCard.tsx
│   │   └── RealTimeChart.tsx
│   ├── layouts/         # Layout components
│   │   └── Navigation.tsx
│   ├── pages/           # Page components
│   │   ├── Dashboard.tsx
│   │   ├── Alarms.tsx
│   │   └── Settings.tsx
│   ├── hooks/           # Custom React hooks
│   │   └── useVentilatorData.ts
│   ├── services/        # External services
│   │   ├── mqtt.service.ts
│   │   └── mockMqtt.service.ts
│   ├── types/           # TypeScript type definitions
│   │   └── index.ts
│   ├── App.tsx          # Main app component
│   └── index.css        # Global styles
├── tailwind.config.js   # TailwindCSS configuration
├── package.json         # Dependencies
└── vite.config.ts       # Vite configuration
```

## Installation

1. Install dependencies:
   ```bash
   npm install
   ```

2. Start development server:
   ```bash
   npm run dev
   ```

3. Build for production:
   ```bash
   npm run build
   ```

4. Preview production build:
   ```bash
   npm run preview
   ```

## Features

### Dashboard Page
- Real-time metric cards (Pressure, Volume, Flow, FR, PEEP, Mode, I:E)
- Status indicators (WiFi, Battery, System Status)
- Real-time graphs for Pressure, Flow, and Volume
- Mock/Real MQTT toggle for testing
- 100-point data buffer for smooth graphing

### Alarms Page
- Alarm history with timestamps
- Severity-based color coding (Critical/Warning/Info)
- Search functionality
- Filter by severity
- Active alarm indicators

### Settings Page
- Ventilation mode selection (VCV/PCV/VAC)
- Respiratory rate adjustment (5-40 breaths/min)
- Tidal volume adjustment (200-800 mL)
- PEEP adjustment (0-20 cmH2O)
- I:E ratio adjustment (1:5 to 1:1)
- Save and reset functionality
- Medical warning banner

## MQTT Integration

### Topics

**Subscribe:**
- `respirateur-medvent/data` - Receives real-time ventilator data

**Publish:**
- `respirateur-medvent/config` - Sends ventilation parameter changes

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

### MQTT Configuration

For local development, copy `.env.example` to `.env` and configure your MQTT broker URL:

```bash
VITE_MQTT_BROKER_URL=wss://broker.hivemq.com:8884/mqtt
VITE_MQTT_USERNAME=
VITE_MQTT_PASSWORD=
```

The dashboard also contains this public demo broker as a code fallback, so Vercel can work without environment variables.

For a private deployed broker, use secure WebSockets:

```bash
VITE_MQTT_BROKER_URL=wss://your-broker-domain.example/mqtt
VITE_MQTT_USERNAME=your-dashboard-user
VITE_MQTT_PASSWORD=your-dashboard-password
```

The ESP32 uses normal MQTT, while the browser dashboard uses MQTT over WebSocket.

## Deployment

1. Build the dashboard:
   ```bash
   npm run build
   ```

2. Deploy the generated `dist/` directory to a static host.

3. Environment variables are optional. If your host supports them, configure:
   ```bash
   VITE_MQTT_BROKER_URL=wss://your-broker-domain.example/mqtt
   VITE_MQTT_USERNAME=your-dashboard-user
   VITE_MQTT_PASSWORD=your-dashboard-password
   ```

4. If no environment variables are configured, the dashboard uses:
   ```bash
   wss://broker.hivemq.com:8884/mqtt
   ```

5. Make sure the MQTT broker is reachable from the user's browser, not only from the ESP32.

The project includes:
- `public/_redirects` for Netlify-style static hosting
- `vercel.json` for Vercel SPA routing

## ESP32 Integration Contract

Give these requirements to the ESP32/electromechanics side.

### Broker

- ESP32 connects to the MQTT broker over TCP, usually port `1883`.
- Dashboard connects to the same broker over WebSocket, usually `ws://...:8083/mqtt` locally or `wss://.../mqtt` in deployment.
- If the dashboard is served over HTTPS, the MQTT WebSocket URL must use `wss://`, otherwise browsers may block the connection.

### Topics

ESP32 publishes telemetry:

```text
respirateur-medvent/data
```

ESP32 subscribes to remote settings:

```text
respirateur-medvent/config
```

### Telemetry Payload: ESP32 to Dashboard

Publish JSON on `respirateur-medvent/data`:

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

Expected fields:
- `pressure`: number, cmH2O
- `flow`: number, L/min
- `volume`: number, mL
- `fr`: number, breaths/min
- `peep`: number, cmH2O
- `mode`: `"VCV"`, `"PCV"`, or `"VAC"`
- `alarm`: boolean
- `battery`: number, percent from `0` to `100`
- `wifi`: boolean

Recommended publish rate: `100 ms` to `500 ms`.

### Config Payload: Dashboard to ESP32

The dashboard publishes JSON on `respirateur-medvent/config`:

```json
{
  "mode": "VCV",
  "fr": 15,
  "volume": 500,
  "peep": 5,
  "ie_ratio": 0.5
}
```

Expected handling:
- Validate all values on the ESP32 before applying them.
- Apply only medically/safely allowed ranges.
- Acknowledge or log rejected settings on the ESP32 side.
- Do not rely on the dashboard as a safety controller.

## Mock Mode

The dashboard includes a mock MQTT service for testing without hardware:

- Generates realistic ventilator data waveforms
- Simulates breathing cycles (4-second period)
- Simulates alarm conditions
- Updates every 100ms

Toggle between Mock and Real MQTT using the button in the dashboard header.

## Color Scheme

- **Background:** `#0a0a0f` (Very dark gray)
- **Cards:** `#1a1a2e` (Night blue)
- **Card Light:** `#16213e` (Lighter night blue)
- **Accent:** `#0f3460` (Deep blue)
- **Primary:** `#00d4ff` (Cyan)
- **Success:** `#00ff00` (Green)
- **Warning:** `#ffaa00` (Orange)
- **Danger:** `#ff0000` (Red)

## Components

### MetricCard

Reusable card component for displaying metrics with status indicators.

```tsx
<MetricCard
  title="Pressure"
  value="22.5"
  unit="cmH₂O"
  status="normal"
/>
```

### RealTimeChart

Real-time line chart component using Recharts with 100-point buffer.

```tsx
<RealTimeChart
  data={pressureData}
  color="#00d4ff"
  title="Pressure"
  unit="cmH₂O"
  yDomain={[0, 40]}
/>
```

## Custom Hooks

### useVentilatorData

Custom hook for managing ventilator data connection and updates.

```tsx
const { data, connected, isMock, setIsMock } = useVentilatorData({ useMock: true });
```

## TypeScript Types

All types are defined in `src/types/index.ts`:

- `VentilatorData` - Real-time ventilator parameters
- `VentilatorSettings` - Configuration settings
- `Alarm` - Alarm information
- `HistoryEvent` - Event log entries
- `GraphDataPoint` - Graph data point
- `VentilationMode` - VCV | PCV | VAC
- `AlarmSeverity` - info | warning | critical

## Building for Production

1. Build the project:
   ```bash
   npm run build
   ```

2. The output will be in the `dist/` directory

3. Deploy to any static hosting service (Vercel, Netlify, etc.)

## Development

### Adding New Pages

1. Create page component in `src/pages/`
2. Add route in `src/App.tsx`
3. Add navigation link in `src/layouts/Navigation.tsx`

### Adding New Metrics

1. Add to `VentilatorData` type in `src/types/index.ts`
2. Add `MetricCard` to Dashboard page
3. Update mock data generator if needed

### Customizing Colors

Edit `tailwind.config.js` to customize the color scheme:

```javascript
theme: {
  extend: {
    colors: {
      // Add or modify colors here
    }
  }
}
```

## Troubleshooting

### MQTT Connection Issues
- Verify broker URL in `mqtt.service.ts`
- Check firewall settings
- Ensure broker supports WebSocket connections

### Graphs Not Updating
- Check data callback in `useVentilatorData` hook
- Verify data structure matches expected format
- Check browser console for errors

### Styling Issues
- Ensure TailwindCSS is properly configured
- Check `tailwind.config.js` content paths
- Verify TailwindCSS directives in `index.css`

## Browser Compatibility

- Chrome/Edge (recommended)
- Firefox
- Safari
- Modern browsers with ES6+ support

## Performance

- Graphs maintain 100-point buffer for smooth rendering
- Data updates every 100ms in mock mode
- Optimized with React hooks and memoization
- Lazy loading of components with React Router

## Security Notes

- MQTT credentials should be stored in environment variables
- Use WSS (WebSocket Secure) in production
- Implement authentication for production deployment
- Validate all incoming MQTT data

## License

This project is part of an academic project for demonstration purposes.
