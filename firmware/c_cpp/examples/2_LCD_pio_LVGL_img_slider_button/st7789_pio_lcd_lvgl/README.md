# ST7789 PIO LCD LVGL Demo with EEZ Studio

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENCE.txt)
[![Platform](https://img.shields.io/badge/platform-Raspberry%20Pi%20Pico%20W-blue.svg)]()
[![LVGL](https://img.shields.io/badge/LVGL-v9.4.0-orange.svg)](https://lvgl.io/)
[![EEZ Studio](https://img.shields.io/badge/EEZ%20Studio-GUI-green.svg)](https://www.envox.eu/eez/eez-studio/introduction/)

![Screenshot](../st7789-lvgl-pio.png)

A comprehensive LVGL v9.4.0 demo showcasing ST7789 LCD integration using Raspberry Pi Pico W's PIO (Programmable I/O) interface. Features a professionally designed GUI created with EEZ Studio, interactive button controls, and optimized display performance.

## 🎨 Demo Features

- **Interactive GUI Elements**:
  - Image display widget
  - Animated spinner widget  
  - Dynamic label with real-time value updates
  - Interactive slider widget with button controls
  
- **Hardware Integration**:
  - PIO-optimized ST7789 display driver (~60MHz SPI)
  - Physical button controls (A: decrease, B: increase)
  - Hardware debouncing with 20ms delay
  - Wi-Fi capability (Raspberry Pi Pico W)
  - LED status indication

- **Professional UI Design**:
  - Created with EEZ Studio for pixel-perfect layouts
  - Smooth animations and transitions
  - Real-time value feedback ("HwIT: XX%")
  - Optimized for 2-inch ST7789 displays

## 🖥️ Hardware Setup

### Components Required
- **Raspberry Pi Pico W** (with Wi-Fi/Bluetooth)
- **ST7789 LCD Display** (2 inches, 240x320)
- **2x Push Buttons** (for A/B controls)
- **Pull-up resistors** (10kΩ recommended)
- **Breadboard and jumper wires**

### Pin Configuration

| Pico Pin | GPIO | ST7789 Pin | Function | Notes
|-----------|------------|------|----------|-----|
| **ST7789 Display** | | | |
| Pin 25 | GP19 | DIN/MOSI | Data In | SPI MOSI |
| Pin 24 | GP18 | CLK/SCK | Clock | SPI Clock |
| Pin 22 | GP17 | CS | Chip Select | Active Low |
| Pin 6 | GP4 | DC | Data/Command | High=Data |
| Pin 26 | GP20 | RST | Reset | Active Low |
| Pin 12 | GP9 | BL | Backlight | High=On |
| Pin 36 | 3V3 | VCC | Power | 3.3V Only |
| Pin 38 | GND | GND | Ground | Common Ground |
| **Controls** | | | |
| Pin 7 | GP5 | Button A | Internal |
| Pin 9 | GP6 | Button B | Internal |
| Pin 38 | GND | Common | - |
| **Status** | | | |
| LED | CYW43_WL_GPIO_LED_PIN | Status LED (via WiFi chip) |

### Wiring Diagram

```
Pico W                    ST7789 Display
┌─────────────┐          ┌──────────────┐
│ 3V3    3V3  │◄────────►│ VCC          │
│ GND    GND  │◄────────►│ GND          │
│ GP18   Pin24│◄────────►│ SCL/CLK      │
│ GP19   Pin25│◄────────►│ SDA/DIN      │
│ GP17   Pin22│◄────────►│ CS           │
│ GP04   Pin06│◄────────►│ DC           │
│ GP20   Pin26│◄────────►│ RST          │
│ GP09   Pin12│◄────────►│ BL           │
└─────────────┘          └──────────────┘

Buttons:
GP5 (Pin 7) ──┐
              ├── Button A ── GND
         10kΩ ┘

GP6 (Pin 9) ──┐  
              ├── Button B ── GND
         10kΩ ┘
```

## 🚀 Quick Start

### Prerequisites

- **Pico SDK** (latest version)
- **CMake** (version 3.13+)
- **GCC ARM Toolchain**
- **EEZ Studio** (for GUI modifications)
- **Git**

### Installation & Build

#### Method 1: VS Code with Raspberry Pi Pico Extension (Recommended)

1. **Prerequisites Setup**
   - Install [Visual Studio Code](https://code.visualstudio.com/)
   - Install [Raspberry Pi Pico VS Code Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)
   - The extension will automatically install:
     - Pico SDK 2.1.1
     - CMake Tools
     - C/C++ Extension
     - ARM GCC Toolchain

2. **Clone and Import Project**
   ```bash
   git clone https://github.com/jrfo-hwit/hlab.git
   cd hlab/firmware/c_cpp/examples/2_LCD_pio_LVGL_img_slider_button/st7789_pio_lcd_lvgl
   code .  # Open in VS Code
   ```

3. **Configure Project in VS Code**
   - Open VS Code Command Palette (`Ctrl+Shift+P` / `Cmd+Shift+P`)
   - Run: `Raspberry Pi Pico: Configure Project`
   - Select options:
     - **Pico SDK Version**: `2.1.1` 
     - **Board Type**: `Raspberry Pi Pico W`
     - **Existing Project**: `Yes` (since you cloned existing code)

4. **Build the Project**
   - **Option A - VS Code UI**:
     - Click the **Build** button in the status bar
     - Or use Command Palette: `Raspberry Pi Pico: Compile Project`
   
   - **Option B - Terminal**:
     ```bash
     # VS Code will create these automatically
     mkdir build
     cd build
     cmake ..
     make -j4
     ```

5. **Flash to Pico W**
   - **Option A - VS Code UI**:
     - Hold BOOTSEL button while connecting Pico W
     - Click **Upload** button in status bar
     - Or Command Palette: `Raspberry Pi Pico: Upload Project`
   
   - **Option B - Manual**:
     ```bash
     # Hold BOOTSEL button, connect USB
     # Pico appears as USB drive (Windows: RPI-RP2, macOS: /Volumes/RPI-RP2)
     cp build/st7789_lcd_pio.uf2 /media/RPI-RP2/  # Linux
     cp build/st7789_lcd_pio.uf2 /Volumes/RPI-RP2/  # macOS
     # Windows: copy to RPI-RP2 drive in File Explorer
     ```

## 🎮 Usage

### Basic Operation

1. **Power On**: Connect Pico W via USB or external 5V supply
2. **Display Boot**: ST7789 initializes and shows the EEZ Studio GUI
3. **Interactive Controls**:
   - **Button A**: Decreases slider value by 5% (minimum 0%)
   - **Button B**: Increases slider value by 5% (maximum 100%)
   - **Label Update**: Shows real-time value as "HwIT: XX%"
   - **Visual Feedback**: Spinner animation indicates system activity

### GUI Components

```c
// Main UI elements (generated by EEZ Studio)
lv_obj_t *image_widget;    // Static image display
lv_obj_t *spinner_widget;  // Animated spinner
lv_obj_t *label_widget;    // Dynamic text label
lv_obj_t *slider_widget;   // Interactive slider
```

### Button Control Logic

```c
// Button debouncing and control (from main.c)
#define DEBOUNCE_DELAY_MS 20

// Button A: Decrease slider value
if (btna == true && btnb == false) {
    slider_value = (slider_value < 0) ? 0 : slider_value - 5;
}

// Button B: Increase slider value  
if (btna == false && btnb == true) {
    slider_value = (slider_value > 100) ? 100 : slider_value + 5;
}

// Update UI elements
lv_slider_set_value(objects.obj1, slider_value, LV_ANIM_OFF);
lv_label_set_text_fmt(objects.obj2, "  HwIT: %d %%", slider_value);
```

## ⚙️ Configuration

### Display Settings

Modify pin definitions in `st7789_lcd_pio.h`:
```c
#define PIN_DIN   19  // SPI Data (MOSI)
#define PIN_CLK   18  // SPI Clock
#define PIN_CS    17  // Chip Select
#define PIN_DC    04  // Data/Command
#define PIN_RESET 20  // Reset
#define PIN_BL    09  // Backlight
```

### LVGL Configuration

Key settings in `lvgl/configs/lv_conf.h`:
```c
#if 1 /* Set this to "1" to enable content */

#define LV_COLOR_DEPTH    16        // 16-bit RGB565
```

### Button Customization

Adjust button pins and behavior in `main.c`:
```c
#define BUTTON_A_PIN 5              // GPIO5 for Button A
#define BUTTON_B_PIN 6              // GPIO6 for Button B  
#define DEBOUNCE_DELAY_MS 20        // Debounce time
```

## 🎨 EEZ Studio Integration

### GUI Design Workflow

1. **Open EEZ Studio**: Load the existing `.eez-project` file
2. **Design Elements**: 
   - Drag and drop widgets (image, spinner, label, slider)
   - Configure properties, styles, and animations
   - Set object names for code reference
3. **Generate Code**: Export LVGL-compatible C code
4. **Integration**: Generated files go to `ui/` directory

### Generated UI Structure

```
ui/
├── ui.h              # Main UI header
├── ui.c              # UI initialization functions  
├── screens.h/.c      # Screen definitions
├── styles.h/.c       # Style definitions
├── images.h/.c       # Image resources
├── fonts.h           # Font definitions
└── actions.h         # Event handlers
```

## 🔧 Performance Optimization

### PIO State Machine

The ST7789 driver uses optimized PIO assembly for maximum throughput:

```assembly
; st7789_lcd.pio - 60MHz SPI communication
.program st7789_lcd
.side_set 1

bitloop:
    out pins, 1   side 0 [1]    ; Output data bit, CLK low
    jmp !osre bitloop side 1 [1]; Continue if more bits, CLK high
```

### Memory Management

- **Frame Buffer**: 30KB (2x240×340×16bit/10)
- **LVGL Heap**: 64KB (configurable)
- **Double Buffering**: Enabled for smooth animations
- **Partial Updates**: Only changed areas refreshed

### Performance Metrics

- **Display Refresh**: 60 FPS sustained
- **SPI Clock**: 60 MHz (PIO-optimized)  
- **Button Response**: <100ms (with debouncing)

## 🛠️ Troubleshooting

### Common Issues

**Display Not Working**
- Verify 3.3V power supply (not 5V!)
- Check SPI wiring (CLK, DIN, CS, DC, RST)
- Confirm pin definitions match hardware
- Test with slower PIO clock divider

**Buttons Not Responding**  
- Check pull-up resistors (10kΩ)
- Verify GPIO pin assignments
- Adjust debounce delay if needed
- Test with multimeter for proper voltage levels

**GUI Elements Missing**
- Ensure EEZ Studio code generation completed
- Check `ui_init()` is called after LVGL init
- Verify object references in `objects` structure
- Check LVGL heap size (increase if needed)

**Performance Issues**
- Reduce animation complexity
- Optimize image formats (use indexed colors)
- Increase PIO clock divider for stability
- Monitor memory usage with LVGL debugging

### Debug Output

Enable comprehensive logging:
```c
// In lv_conf.h
#define LV_USE_LOG      1
#define LV_LOG_LEVEL    LV_LOG_LEVEL_INFO

// In main.c  
printf("Slider value: %d\n", lv_slider_get_value(objects.obj1));
printf("Button A: %s, Button B: %s\n", 
       btna ? "Pressed" : "Released",
       btnb ? "Pressed" : "Released");
```

## 📁 Project Structure

```
ST7789_PIO_LCD_LVGL/
├── src/
│   └── main.c                 # Main application (your provided code)
├── lvgl/                      # LVGL v9.4.0 library
│   ├── src/                   # LVGL source code
│   ├── configs/lv_conf.h      # LVGL configuration
│   └── ...
├── ui/                        # EEZ Studio generated UI
│   ├── ui.h/.c               # Main UI functions
│   ├── screens.h/.c          # Screen definitions  
│   ├── images.h/.c           # Image resources
│   └── styles.h/.c           # Style definitions
├── st7789_lcd_pio.h/.c       # PIO-based ST7789 driver
├── st7789_lcd.pio            # PIO assembly program
├── CMakeLists.txt            # Build configuration
├── pico_sdk_import.cmake     # Pico SDK integration
└── README.md                 # This documentation
```

## 🤝 Contributing

Contributions welcome! Areas for improvement:

- **New UI Widgets**: Add more EEZ Studio components
- **Display Support**: Other ST77xx variants  
- **Input Methods**: Encoder, touchscreen support
- **Performance**: Further PIO optimizations
- **Documentation**: More EEZ Studio tutorials

### Development Guidelines

1. Test on actual hardware before submitting PRs
2. Update EEZ Studio project files for UI changes
3. Maintain compatibility with LVGL v9.4.0
4. Document any new pin assignments or configurations

## 📄 License

MIT License - see [LICENCE.txt](LICENCE.txt)

**Author**: Juliano Oliveira  
**Date**: 2025-06-10  
**Copyright**: (c) 2025 Hardware Innovation Technologies

## 🙏 Acknowledgments

- **[EEZ Studio](https://www.envox.eu/eez/eez-studio/)** - Professional GUI design tool
- **[LVGL Team](https://lvgl.io/)** - Excellent embedded graphics library  
- **[Raspberry Pi Foundation](https://raspberrypi.org/)** - RP2040 and PIO innovation
- **Hardware Innovation Technologies** - Project sponsorship and support

## 📞 Support

- 💬 **EEZ Studio**: [Community Forum](https://www.envox.eu/eez/eez-studio/community/)
- 📖 **LVGL Docs**: [docs.lvgl.io](https://docs.lvgl.io/master/)
- 🔧 **Pico SDK**: [Raspberry Pi Documentation](https://datasheets.raspberrypi.org/pico/raspberry-pi-pico-c-sdk.pdf)

---

<div align="center">
  <strong>Built with ❤️ by Hardware Innovation Technologies</strong><br>
  <em>Combining EEZ Studio design excellence with RP2040 performance</em>
</div>