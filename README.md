# 🦾 ESP32 Robot Arm Controller

A 3-axis robot arm controller built with ESP32, featuring manual joystick control, dual speed modes, and a record/playback system for repeating movement sequences.

---

## 📋 Features

- **3-axis manual control** – joystick for X/Y axes, buttons for Z axis
- **Dual speed mode** – toggle between fast and slow movement via joystick click
- **Movement recording** – save up to 30 position steps with a single button press
- **Smooth playback** – recorded sequences play back with linear interpolation between positions
- **Long-press reset** – hold record button for 3 seconds to clear all saved steps
- **Non-blocking logic** – all button handling uses edge detection, no `delay()` in control flow

---

## 🔧 Hardware

| Component | Quantity | Notes |
|---|---|---|
| ESP32 DevKit | 1 | Main controller |
| SG90 Servo | 3 | Axes: X, Y, Z |
| Analog Joystick | 1 | X/Y axes + click |
| Push Button | 4 | Z-up, Z-down, Record, Playback |

---

## 📌 Pin Connections

| Pin | Function |
|---|---|
| GPIO 17 | Servo 1 (X axis) |
| GPIO 18 | Servo 2 (Y axis) |
| GPIO 19 | Servo 3 (Z axis) |
| GPIO 34 | Joystick X (analog) |
| GPIO 35 | Joystick Y (analog) |
| GPIO 32 | Joystick button (speed toggle) |
| GPIO 13 | Button Z-down |
| GPIO 14 | Button Z-up |
| GPIO 16 | Record button |
| GPIO 21 | Playback button |

---

## 📦 Dependencies

- [ESP32Servo](https://github.com/madhephaestus/ESP32Servo) – servo control library for ESP32

Install via Arduino Library Manager: search **ESP32Servo**

---

## 🚀 How to Use

### Manual Control
- **Joystick left/right** → rotates arm on X axis
- **Joystick forward/back** → moves arm on Y axis
- **Z-up / Z-down buttons** → moves arm on Z axis
- **Joystick click** → toggles between fast (±10) and slow (±2) movement speed

### Recording a Sequence
1. Move the arm to a desired position
2. **Press Record button** → saves current position as a step (up to 30 steps)
3. Repeat for each position in the sequence
4. **Press Playback button** → arm smoothly moves through all saved steps
5. **Hold Record button for 3 seconds** → resets all saved steps

---

## 🧠 Technical Highlights

- Manual debouncing via edge detection (`HIGH → LOW` transitions) without external libraries
- Long-press detection using `millis()` timestamp comparison
- Smooth interpolation during playback – each servo moves at a fixed step rate toward the next target position
- Position storage in a 2D array `int zapis_pozycji[30][3]` (steps × axes)
- `static` local variables used for persistent state within functions

---

## 📁 Files

| File | Description |
|---|---|
| `robot_arm_controller.ino` | Main controller – current working version |
| `robot_arm_legacy.ino` | Earlier version – playback not functional |
| `position_rw_test.ino` | Standalone test for position save/load logic |

---

## 🔌 Wiring Diagram

> *Schematic coming soon*

---

## 📄 License

MIT License – free to use, modify and distribute.
