# 🛡️ LADAKH-SHIELD
### High-Altitude Electronic System Monitoring & Adaptive Protection

> **Defence-Grade Embedded Telemetry & Adaptive Protection Cockpit for Extreme Alpine Operational Environments**  
> Tailored for the harsh conditions of Ladakh (~4,850m AMSL, -40°C temperatures, 50–58 kPa barometric pressure).

---

## 🏔️ Concept & Architectural Overview

In extreme high-altitude military deployment zones such as Ladakh, Siachen, and eastern mountain passes, electronic equipment faces three severe hazards:
1. **Electrolyte Freezing & Thermal Contraction**: Temperatures plunging below -20°C to -40°C cause severe battery capacity collapse and component brittleness.
2. **Rarefied Atmosphere & Derated Convection**: Low atmospheric pressure (~50 kPa at 5,000m) reduces air density, diminishing convective fan cooling efficiency by over 35%.
3. **Severe Electrical Load Surges**: Cold motor starts and iced mechanical actuators draw heavy inrush currents that can destroy flight electronics and power rails.

### The Solution: One Universal Smart Module
Instead of building five disparate monitoring units, **LADAKH-SHIELD** implements **ONE UNIVERSAL SMART MODULE** that attaches to five distinct high-altitude assets:
- 🚁 **Tactical Drone / UAV Subsystem**: ESC temperatures, flight avionics, rotor current, battery state-of-charge.
- 📡 **Border Surveillance Radar**: T/R module thermal envelope, beam sweep power, RF amplifier voltages.
- 📻 **VHF/UHF Tactical Radio**: Power amplifier stage temperature, VSWR, signal RSSI, encrypted mesh link.
- 💻 **Tactical Edge Computer**: Multi-core CPU thermal monitoring, computing power draw, RAM allocation.
- 🔋 **Cold-Resistant Power Pack**: Cell temperature, pack voltage, discharge rate, health cycle index.

---

## ⚡ Hardware Prototype Stack

| Hardware Component | Function / Role in LADAKH-SHIELD |
|---|---|
| **ESP32 DevKit v1** | Main Dual-Core 240MHz System Controller |
| **Bosch BME280** | I2C Transducer: Ambient/Enclosure Temperature, Barometric Pressure, Humidity |
| **TI INA219 / INA226** | I2C High-Side Current & Bus Voltage Coulomb Counter ($P = V \times I$) |
| **SSD1306 0.96" OLED** | Local field operator status display |
| **Solid-State MOSFETs** | High-speed protection relays: PTC Heater (PWM), Cooling Fan, Main Power Rail Breaker |
| **PTC Ceramic Heater Pad** | Thermal injection preventing sub-zero electrolyte freeze (&lt; -20°C auto trip) |
| **Brushless Cooling Fan** | Thermal regulation preventing heat trap (&gt; 45°C auto trip) |
| **LoRa SX1278 (868MHz)** | Long-range chirp spread spectrum telemetry link (up to 15km mountain line-of-sight) |
| **MicroSD Card SPI** | Autonomous offline flight recorder buffering when RF link is shadowed |
| **Piezo Buzzer + Strobe** | Acoustic and visual emergency fault indication |
| **MySQL 8.0+** | Production relational storage for telemetry, equipment status, alerts, and actuator logs |

---

## 🚀 Getting Started

### 1. Launch Backend Telemetry Server
The backend REST API server runs on Node.js (Port `5000`):
```bash
node server/index.js
```
*API endpoints available:*
- `GET  /api/health`
- `GET  /api/sensor-data`
- `POST /api/sensor-data` *(Receives live ESP32 JSON telemetry)*
- `GET  /api/equipment`
- `POST /api/equipment-status`
- `GET  /api/alerts`
- `POST /api/alerts/acknowledge`
- `GET  /api/protection-status`
- `POST /api/protection-control`
- `GET  /api/logs`
- `GET  /api/export-csv`

### 2. Launch Cockpit Frontend Application
Run the Vite development server (Port `3000`):
```bash
npm run dev
# or for full network access on local network:
npm run start
```
Open **`http://localhost:3000`** in your browser.

---

## 🎯 SIH Presentation Demo Suite

The application includes a prominent **SIH Presentation Demo Suite** bar at the top of the interface, designed specifically for live college evaluations and Smart India Hackathon demonstrations. With 1 click, you can inject real-world high-altitude crisis events:

1. ❄️ **EXTREME COLD**
   - *Reaction:* Temperature drops towards -35°C, atmospheric pressure drops, battery health decreases, the **PTC Heater Pad activates automatically**, and an advisory warning alert is logged.
2. ⚠️ **LOW PRESSURE**
   - *Reaction:* Atmospheric pressure drops to 44 kPa (simulating 5,800m Khardung La pass), warning alert is dispatched, cooling fan efficiency is derated.
3. 🔥 **HIGH TEMPERATURE**
   - *Reaction:* Electronics temperature spikes to +58°C, **Cooling Fan kicks into 4,800 RPM**, thermal overload critical alert is dispatched.
4. 🔋 **LOW BATTERY**
   - *Reaction:* Bus voltage plunges to 9.6V (14% SoC), undervoltage alarm triggers, load-shedding protocol is recommended.
5. ⚡ **OVERCURRENT**
   - *Reaction:* Shunt current spikes abruptly to 7.85A, **Solid-State MOSFET Breaker trips in milliseconds**, power rail is isolated, critical buzzer alarm sounds.
6. 📡 **COMMUNICATION FAILURE**
   - *Reaction:* LoRa RF link collapses, packet loss hits 100%, status indicator turns red, **local SD Card offline fallback** activates automatically.
7. 🔄 **RESET NOMINAL**
   - *Reaction:* Restores system to stable nominal high-altitude operational baseline (-24.5°C, 58.2 kPa, 12.4V, 2.6A).

---

## 📁 Project Directory Map

```
ladakh-shield/
├── schema.sql                         # Complete MySQL production database schema
├── esp32_firmware_sample.ino          # Production Arduino/ESP32 C++ firmware sketch
├── server/
│   └── index.js                       # Node.js ES Module REST API telemetry server
├── src/
│   ├── types/
│   │   └── telemetry.ts               # Strict TypeScript definitions for all telemetry
│   ├── engine/
│   │   ├── riskEngine.ts              # Deterministic rule-based risk evaluation
│   │   ├── protectionEngine.ts        # Adaptive heating, cooling & MOSFET trip controller
│   │   └── simulationEngine.ts        # Ladakh environmental physics and scenario engine
│   ├── context/
│   │   └── TelemetryContext.tsx       # Central state provider, ticking loop, audio synth
│   ├── components/
│   │   ├── layout/
│   │   │   ├── Header.tsx             # Cockpit HUD header, ticking clock, status badges
│   │   │   ├── Sidebar.tsx            # Left navigation bar with live actuator status
│   │   │   ├── BottomStatusBar.tsx    # Persistent 4,850m location, ID & firmware bar
│   │   │   └── DemoModeBar.tsx        # 1-click SIH presentation scenario switcher
│   │   ├── common/
│   │   │   ├── StatusBadge.tsx        # Color-coded tactical status pill
│   │   │   ├── GaugeMetric.tsx        # Cockpit arc HUD circular gauge
│   │   │   ├── MiniSparkline.tsx      # SVG trend line
│   │   │   ├── Modal.tsx              # Reusable cockpit modal dialog
│   │   │   └── EquipmentDetailModal.tsx # Deep inspection dialog for all 5 subsystems
│   │   └── pages/
│   │       ├── DashboardPage.tsx      # Environment, Power, Equipment, Risk & Subsystem integrity
│   │       ├── SensorsPage.tsx        # 6 dynamic time-series charts with 1H/6H/24H/7D filters
│   │       ├── EquipmentPage.tsx      # Subsystem monitoring for Drone, Radar, Radio, Computer, Battery
│   │       ├── CommunicationPage.tsx  # LoRa SX1278 link telemetry, pipeline visualizer & hex viewer
│   │       ├── ProtectionPage.tsx     # Adaptive Heater, Cooling Fan, MOSFET & Emergency Shed
│   │       ├── DataLogsPage.tsx       # Filterable table, SD card gauge & CSV/JSON export
│   │       ├── AlertsPage.tsx         # Real-time alert center with acknowledge & clear actions
│   │       └── SettingsPage.tsx       # REST API tester, calibration offsets & MySQL viewer
```

---

## 🛡️ Presentation Narrative

```
LADAKH EXTREME CONDITIONS (-40°C, 4850m AMSL, Low Pressure)
                    ↓
   MULTIPLE TACTICAL DEFENCE EQUIPMENT (Drone, Radar, Radio, PC, Battery)
                    ↓
   ONE UNIVERSAL SMART MODULE (ESP32 + BME280 + INA219 + LoRa)
                    ↓
   SENSE & MONITOR (Real-time Thermal, Electrical, Barometric telemetry)
                    ↓
   ANALYZE & DECIDE (Deterministic Rule-Based Risk Engine)
                    ↓
   ADAPTIVE PROTECTION (PTC Heater, PWM Cooling Fan, Solid-State MOSFET)
                    ↓
   ALERT + DUAL LOGGING (LoRa Telemetry + Local SD Card + MySQL)
                    ↓
   MISSION SUCCESS & RELIABLE OPERATION
```
