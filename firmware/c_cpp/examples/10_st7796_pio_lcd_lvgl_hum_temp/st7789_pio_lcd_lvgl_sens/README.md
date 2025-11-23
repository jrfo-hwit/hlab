# AHT10 + ST7796 LCD + LVGL Temperature, Humidity & Dew Point Monitor

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Platform](https://img.shields.io/badge/Platform-Raspberry%20Pi%20Pico%20W-green.svg)](https://www.raspberrypi.com/products/raspberry-pi-pico/)
[![LVGL](https://img.shields.io/badge/LVGL-v9.4.0-blue.svg)](https://lvgl.io/)
[![EEZ Studio](https://img.shields.io/badge/EEZ%20Studio-UI%20Designer-purple.svg)](https://www.envox.hr/eez/studio/studio-introduction.html)
[![Display](https://img.shields.io/badge/Display-ST7796%203.5%22-orange.svg)](https://www.sitronix.com.tw/)

## 📖 Overview

This project demonstrates a complete LVGL v9.4.0 (designed in EEZ Studio) graphical user interface implementation of a real-time environmental monitoring system built with Raspberry Pi Pico W, featuring a 3.5-inch **ST7796 LCD display** (480x320 pixels). The system displays live monitoring values of temperature, humidity, and dew point with professional-grade environmental monitoring on the main screen and 1-day measurement history on chart readings in the second screen.

This is an enhanced version based on the ST7789 project (example 9), optimized for the larger **ST7796 480x320 display** with **memory-efficient single buffer** implementation.

## 🎯 Key Differences from ST7789 Version (Example 9)

| Feature | ST7789 (Example 9) | ST7796 (This Project) |
|---------|-------------------|----------------------|
| **Display Controller** | ST7789 | **ST7796** |
| **Resolution** | 320x240 pixels | **480x320 pixels** |
| **Display Size** | 2 inches | **3.5 inches** |
| **Screen Orientation** | Landscape (90°) | **Landscape (270°)** |
| **Buffer Strategy** | Double buffering | **Single buffer** (memory optimized) |
| **Memory Usage** | ~30KB (2 buffers) | **~15KB (1 buffer)** |
| **Init Sequence** | ST7789 specific | **ST7796 specific** |
| **Color Inversion** | Enabled (0x21) | **Disabled (0x20)** |

## 📋 Features

- **Real-time Monitoring**: Continuous temperature, humidity, and dew point measurements
- **Large Display**: 3.5-inch ST7796 LCD with 480x320 resolution for better visibility
- **Visual Feedback**: Color-coded arc indicators showing normal, high, and low ranges
- **Graphical History**: 24-hour trend chart with 30-minute sampling intervals
- **Dual Screen Interface**:
  - Main screen with live monitoring values of temperature, humidity and dew point
  - Graph screen with historical data of 1-day measurement (each point measured every 30 minutes)
- **Button Navigation**: Two-button interface (A and B) for screen switching
- **PIO-Optimized Display Driver**: High-performance ST7796 driver using RP2040's PIO peripheral
- **Memory Efficient**: Single buffer implementation reduces RAM usage by 50%
- **Demo Mode**: Automatic screen switching every 7 seconds for demonstration purposes
- **Professional Design**: Clean UI with high contrast elements
- **Dual-core RP2040** processor utilization
- **Critical section protection** for thread safety

## 🛠️ Hardware Requirements

### Components
- **Microcontroller**: Raspberry Pi Pico W (with DEBUG port)
- **Development Board**: HWIT custom PCB with integrated components
- **Display**: 3.5-inch ST7796 LCD (480x320 pixels) with carrier board
- **Sensor**: AHT10 Temperature & Humidity Sensor module
- **Controls**:
  - 2x Navigation buttons (tactile push buttons)
- **Connectivity**: SPI interface for display, I2C for sensor
- **Pull-up Resistors**: For I2C (4.7kΩ recommended) and button inputs (10kΩ)

### Pin Connections

#### ST7796 LCD Display
| LCD Pin | Pico Pin | GPIO | Description |
|---------|----------|------|-------------|
| DIN | 25 | GPIO19 | Data Input (MOSI) |
| CLK | 24 | GPIO18 | Clock (SCK) |
| CS | 22 | GPIO17 | Chip Select |
| DC | 6 | GPIO4 | Data/Command |
| RST | 26 | GPIO20 | Reset |
| BL | 12 | GPIO9 | Backlight |
| VCC | 3V3 | - | Power Supply (3.3V) |
| GND | GND | - | Ground |

#### AHT10 Sensor (I2C)
| Sensor Pin | Pico Pin | GPIO | Description |
|------------|----------|------|-------------|
| SDA | 4 | GPIO2 | I2C Data |
| SCL | 5 | GPIO3 | I2C Clock |
| VCC | 3V3 | - | Power Supply |
| GND | GND | - | Ground |

#### Control Buttons
| Button | Pico Pin | GPIO | Function |
|--------|----------|------|----------|
| Button A | 7 | GPIO5 | Back to Main Screen |
| Button B | 9 | GPIO6 | Go to Graph Screen |

#### Wiring Diagram

```
Pico W                    ST7796 Display
┌─────────────┐          ┌──────────────┐
│ 3V3    3V3  │─────────►│ VCC          │
│ GND    GND  │─────────►│ GND          │
│ GP18   Pin24│◄────────►│ SCL/CLK      │
│ GP19   Pin25│◄────────►│ SDA/DIN      │
│ GP17   Pin22│─────────►│ CS           │
│ GP04   Pin06│─────────►│ DC           │
│ GP20   Pin26│─────────►│ RST          │
│ GP09   Pin12│─────────►│ BL           │
│             │          └──────────────┘
│             │           AHT10 Sensor
│             │          ┌──────────────┐
│ 3V3    3V3  │─────────►│ VCC          │
│ GND    GND  │─────────►│ GND          │
│ GP3    Pin5 │◄────────►│ SCL          │
│ GP2    Pin4 │◄────────►│ SDA          │
│             │          └──────────────┘
│             │           Button A
│             │          ┌──────────────┐
│ GP5    Pin7 │◄─────────│ GND          │
│ 10kΩ PULLUP │          └──────────────┘
│             │           Button B
│             │          ┌──────────────┐
│ GP6    Pin9 │◄─────────│ GND          │
│ 10kΩ PULLUP │          └──────────────┘
└─────────────┘
```

## 📁 Project Structure

```
hlab/
├── firmware/
│   └── c_cpp/
│       └── examples/
│           └── 10_st7796_pio_lcd_lvgl_hum_temp/
│               └── st7789_pio_lcd_lvgl_sens/    # Firmware folder
│                   ├── main.c                   # Main application logic
│                   ├── aht10.c/.h               # AHT10 sensor driver
│                   ├── st7789_lcd_pio.c/.h      # ST7796 display driver
│                   ├── st7789_lcd.pio           # PIO assembly for display
│                   ├── lv_conf.h                # LVGL configuration
│                   ├── CMakeLists.txt           # Build configuration
│                   ├── lvgl/                    # LVGL library (submodule)
│                   ├── ui/                      # Generated UI files
│                   │   ├── ui.c/.h              # UI initialization
│                   │   ├── screens.c/.h         # Screen definitions
│                   │   ├── actions.h            # UI actions
│                   │   ├── fonts.h              # Custom fonts
│                   │   ├── images.c/.h          # Image resources
│                   │   ├── styles.c/.h          # UI styles
│                   │   ├── structs.h            # UI structures
│                   │   ├── vars.h               # UI variables
│                   │   └── ui_image_*.c         # Screen-specific images
│                   ├── release/                 # Pre-compiled binaries
│                   │   └── st7789_pio_lcd_lvgl_sens1.uf2
│                   └── README.md                # This file
└── [other hlab projects and resources]
```

## 🎨 GUI Features

### Main Screen (Live Monitoring)
- **HWIT Logo**: Centered atomic-style logo with company branding
- **Temperature Arc**: Red gradient arc displaying current temperature (-50°C to 100°C range)
  - Blue: < 20°C (Low)
  - Green: 20-25°C (Normal)
  - Red: > 25°C (High)
- **Humidity Arc**: Blue/green gradient arc showing relative humidity (0-100% range)
  - Blue: < 30% (Low)
  - Green: 30-60% (Normal)
  - Red: > 60% (High)
- **Dew Point Display**: Center section with icon and calculated value
- **Real-time Values**: Large numeric displays (format: XX.XC / XX.X%)
- **Visual Icons**: Temperature thermometer, humidity droplet, and dew point icons
- **Navigation Hint**: "B >>" indicator for graph screen access

### Graph Screen
- **Header Bar**: Shows button functions ("<<A" for back, "B = Scale Change")
- **Current Reading Status**: Progress indicator (0-100%) in top bar (+1% each 18 seconds)
- **Dual Y-Axis**:
  - Left (Red): Temperature scale (auto-adjusted)
  - Right (Blue): Humidity scale (auto-adjusted)
- **Legend**: Color-coded indicators for Temperature (red) and Humidity (blue)
- **24-Hour History**: Line graph with 48 data points (30-minute intervals)
- **Auto-Scaling**: Dynamic range adjustment based on min/max values
- **Time Scale Display**: "24h@1p/30m" (24 hours at 1 point per 30 minutes)
- **Statistics**: Min, max, and average values displayed

## 🔧 Software Configuration

### LVGL Settings (v9.4.0)
- **Color Depth**: 16-bit RGB565
- **Display Resolution**: 480x320 pixels
- **Rotation**: 270 degrees (landscape mode)
- **Buffer Size**: 1/10 screen size, **single buffered** (15360 bytes = 480px * 320px * 16bits / 8bits / 10)
- **Memory Pool**: 64KB (available for lv_malloc function)
- **Render Mode**: Partial rendering (memory efficient)

### ST7796 Specific Configuration

The ST7796 initialization sequence in `st7789_lcd_pio.h`:

```c
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320

// ST7796 rotation values
#define ST7796_ROTATION_0   0x48   // Portrait (320x480) - MX + BGR
#define ST7796_ROTATION_90  0x28   // Landscape (480x320) - MV + BGR
#define ST7796_ROTATION_180 0x88   // Portrait inverted (320x480) - MY + BGR
#define ST7796_ROTATION_270 0xE8   // Landscape inverted (480x320) - MY + MX + MV + BGR
#define ST7796_ROTATION ST7796_ROTATION_270

static const uint8_t st7789_init_seq[] = {
    1, 20, 0x01,                        // Software reset
    1, 10, 0x11,                        // Exit sleep mode
    2, 2, 0x3a, 0x55,                   // Set colour mode to 16 bit
    2, 0, 0x36, ST7796_ROTATION,        // Set MADCTL
    5, 0, 0x2a, 0x00, 0x00, SCREEN_WIDTH >> 8, SCREEN_WIDTH & 0xff,   // CASET
    5, 0, 0x2b, 0x00, 0x00, SCREEN_HEIGHT >> 8, SCREEN_HEIGHT & 0xff, // RASET
    1, 2, 0x20,                         // Inversion off (different from ST7789!)
    1, 2, 0x13,                         // Normal display on
    1, 2, 0x29,                         // Main screen turn on
    0                                   // Terminate list
};
```

### Sensor Configuration
- **I2C Speed**: 100 kHz
- **I2C Port**: I2C1 (GPIO2 SDA, GPIO3 SCL)
- **Sampling Rate**: Real-time updates (approximately 10ms loop)
- **Chart Update**: Every 30 minutes
- **Temperature Range**: -50°C to 100°C
- **Humidity Range**: 0% to 100%

### Demo Mode
```c
#define DEMO_MODE 1  // Set to 1 to enable, 0 to disable
```
When enabled, the system automatically switches between main and graph screens every 7 seconds.

## 💻 Software Dependencies

### Required Tools
- **[Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)** v2.1.1+
- **[CMake](https://cmake.org/)** 3.13+
- **[Python](https://www.python.org/)** 3.8+
- **[EEZ Studio](https://github.com/eez-open/studio/releases)** 0.23.2+

### Included Libraries
- **LVGL v9.4.0** - Graphics library (in `/lvgl` folder)
- **Pico SDK** - Hardware abstraction layer
- **CYW43 Driver** - WiFi chip control (for LED)

## 🔨 EEZ Studio UI Integration

### Required Modifications to Generated Files

When generating UI files from EEZ Studio, several modifications are necessary for proper integration with the Raspberry Pi Pico firmware:

#### 1. Header File Guards (`ui/*.h` files)
All generated header files need proper conditional compilation guards:

```c
// Add at the beginning of each .h file (e.g., ui.h, screens.h, styles.h, images.h, actions.h)
#ifndef EEZ_LVGL_UI_XXXX_H  // Replace XXXX with appropriate name
#define EEZ_LVGL_UI_XXXX_H

#include "../lvgl/lvgl.h"  // Change from <../lvgl/lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// ... existing content ...

#ifdef __cplusplus
}
#endif

#endif // EEZ_LVGL_UI_XXXX_H
```

#### 2. Include Path Updates
Change all LVGL includes from angle brackets to quotes:
```c
// Change from:
#include <../lvgl/lvgl.h>
// To:
#include "../lvgl/lvgl.h"
```

#### 3. UI Image Files (`ui_image_*.c`)
Each image file requires:
```c
#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
    #include "lvgl.h"
#elif defined(LV_BUILD_TEST)
    #include "../lvgl.h"
#else
    #include "../lvgl/lvgl.h"  // Change from "lvgl/lvgl.h"
#endif
```

### File Modification Checklist

When integrating EEZ Studio generated files:

- [ ] Change header includes from `<../lvgl/lvgl.h>` to `"../lvgl/lvgl.h"` in all `.h` files
- [ ] Update all `ui_image_*.c` files with proper includes from `"lvgl/lvgl.h"` to `"../lvgl/lvgl.h"`
- [ ] Test compilation after modifications

## 🚀 Building and Flashing

### Prerequisites
- Raspberry Pi Pico SDK installed
- Visual Studio Code with Raspberry Pi Pico extension
- CMake 3.13 or higher
- ARM GCC compiler
- EEZ Studio (for GUI modifications)
- Git with submodule support

### Cloning the Repository

This project uses Git submodules for dependency management. Follow these steps to properly clone the repository with all its dependencies:

1. **Clone with submodules (Recommended)**
```bash
git clone --recurse-submodules https://github.com/jrfo-hwit/hlab.git
cd hlab
```

2. **Or, if already cloned without submodules**
```bash
git clone https://github.com/jrfo-hwit/hlab.git
cd hlab
git submodule update --init --recursive
```

3. **Verify submodules are properly initialized**
```bash
git submodule status
```
This should show all submodules without a `-` prefix, indicating they're properly checked out.

4. **Update submodules to latest commits (if needed)**
```bash
git submodule update --remote --merge
```

### Build Instructions - Visual Studio Code

#### Prerequisites
- **Visual Studio Code** with [Raspberry Pi Pico Extension](https://marketplace.visualstudio.com/items?itemName=raspberry-pi.raspberry-pi-pico)
- **Pico SDK 2.1.1** (automatically managed by VS Code extension)
- **EEZ Studio** for UI design ([Download](https://github.com/eez-open/studio/releases))

#### Windows Installation

1. **Install VS Code and Pico Extension**
```powershell
# Install VS Code from https://code.visualstudio.com/

# Open VS Code and install the Raspberry Pi Pico extension:
# - Click Extensions (Ctrl+Shift+X)
# - Search "Raspberry Pi Pico"
# - Install the official extension by Raspberry Pi
```

2. **First-time Setup**
   - The Pico extension will automatically download and install:
     - Pico SDK 2.1.1
     - ARM GCC Compiler
     - CMake
     - Ninja build system
     - Python dependencies

3. **Install EEZ Studio**
```powershell
# Download the Windows installer from:
# https://github.com/eez-open/studio/releases/latest
# Run the installer and follow the setup wizard
```

#### Linux/macOS Installation

1. **Install VS Code**
```bash
# Ubuntu/Debian
sudo snap install code --classic

# macOS (using Homebrew)
brew install --cask visual-studio-code
```

2. **Install Pico Extension**
```bash
# Open VS Code and install from marketplace
code --install-extension raspberry-pi.raspberry-pi-pico
```

3. **Install EEZ Studio**
```bash
# Linux (AppImage)
wget https://github.com/eez-open/studio/releases/latest/download/eez-studio-linux-x64.AppImage
chmod +x eez-studio-*.AppImage
./eez-studio-*.AppImage

# macOS
# Download DMG from https://github.com/eez-open/studio/releases/latest
# Mount and drag to Applications
```

#### Method 1: Using VS Code with Raspberry Pi Pico Extension (Recommended)

1. **Open the project in VS Code**
```bash
cd hlab/firmware/c_cpp/examples/10_st7796_pio_lcd_lvgl_hum_temp/st7789_pio_lcd_lvgl_sens
code .
```

2. **Configure and Build**
   - The VS Code Pico extension will detect the `CMakeLists.txt`
   - Click the **Build** button in the status bar
   - Or use Command Palette: `Raspberry Pi Pico: Compile Project`

3. **Flash to Pico W**
   - Hold BOOTSEL button while connecting Pico W
   - Click **Upload** button in status bar
   - Or drag `build/st7789_pio_lcd_lvgl_sens1.uf2` to RPI-RP2 drive

#### Method 2: Manual Build with CMake

1. **Navigate to the project directory**
```bash
cd hlab/firmware/c_cpp/examples/10_st7796_pio_lcd_lvgl_hum_temp/st7789_pio_lcd_lvgl_sens
```

2. **Set up the Pico SDK**
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
```

3. **Create and enter build directory**
```bash
mkdir build
cd build
```

4. **Configure and build**
```bash
cmake ..
make -j4
```

5. **The built files will be in:**

| File | Description | Usage |
|------|-------------|-------|
| `build/st7789_pio_lcd_lvgl_sens1.uf2` | Firmware binary | Drag-and-drop programming |
| `build/st7789_pio_lcd_lvgl_sens1.elf` | Debug symbols | GDB debugging |
| `build/st7789_pio_lcd_lvgl_sens1.bin` | Raw binary | Advanced flashing |
| `build/st7789_pio_lcd_lvgl_sens1.hex` | Intel HEX | Alternative format |

### Using Pre-compiled Binary

A pre-compiled binary is available in the `release/` folder:

```bash
# Navigate to release folder
cd hlab/firmware/c_cpp/examples/10_st7796_pio_lcd_lvgl_hum_temp/st7789_pio_lcd_lvgl_sens/release

# Flash to Pico W
# 1. Hold BOOTSEL button on Pico W
# 2. Connect USB cable
# 3. Copy st7789_pio_lcd_lvgl_sens1.uf2 to RPI-RP2 drive
```

### Debugging Setup (Optional)

If using a debug probe (Picoprobe or compatible):

1. **Connect the debug probe** to the SWD pins on the Pico W
2. **In VS Code**, press `F5` or use "Run and Debug" panel
3. **Select** "Pico Debug" configuration
4. The debugger will flash and start debugging automatically

### Troubleshooting Build Issues

**Common issues and solutions:**

- **CMake not finding Pico SDK**: Ensure `PICO_SDK_PATH` is correctly set in environment or VS Code settings
- **Submodule errors**: Run `git submodule sync` followed by `git submodule update --init --recursive`
- **VS Code extension not detecting project**: Check that you opened the correct folder containing `CMakeLists.txt`
- **Build fails with LVGL errors**: Verify all UI file modifications are applied (see EEZ Studio UI Integration section)
- **Memory allocation failures**: ST7796's larger resolution requires more memory; single buffer mode helps reduce usage

## 📊 Operation

### Button Controls
- **Main Screen**:
  - Button A: No action currently assigned
  - Button B: Switch to Graph screen (indicated by "B >>" on display)

- **Graph Screen**:
  - Button A: Return to Main screen (indicated by "<< A" on display)
  - Button B: Scale change function (indicated by "B = Scale Change")
    - *Note: Scale change feature to be designed (TBD)*

### Visual Indicators
- The HWIT logo provides brand identity and professional appearance
- Arc indicators change color based on environmental conditions
- Icons provide intuitive understanding (thermometer for temp, droplet for humidity, dew point icon)
- The onboard LED indicates system status (on during operation)

### Data Logging
- Temperature and humidity sampled continuously in real-time
- Values displayed with 0.1 precision (XX.XC format for temperature, XX.X% for humidity)
- Chart data points recorded every 30 minutes
- Circular buffer stores 48 samples (24 hours of data)
- Time scale notation: "24h@1p/30m" (24 hours at 1 point per 30 minutes)
- Progress indicator shows percentage to next sample (0-100%, +1% every 18 seconds)

## 🔬 Technical Details

### ST7796 vs ST7789 Display Driver Differences

The ST7796 requires specific initialization and rotation settings:

```c
// ST7796 rotation differs from ST7789
// ST7789 uses simpler rotation values (0x00, 0x60, 0xC0, 0xA0)
// ST7796 uses:
//   0x48 - Portrait (MX + BGR)
//   0x28 - Landscape (MV + BGR)
//   0x88 - Portrait inverted (MY + BGR)
//   0xE8 - Landscape inverted (MY + MX + MV + BGR)

// Color inversion also differs:
// ST7789: 0x21 (inversion ON)
// ST7796: 0x20 (inversion OFF)
```

### PIO Display Driver
The ST7796 driver uses the RP2040's PIO (Programmable I/O) for optimal performance:
- Hardware-accelerated SPI communication
- Configurable clock divider for speed optimization
- Non-blocking data transfers
- Efficient pixel pushing for smooth UI updates
- Same PIO program works for both ST7789 and ST7796

### Memory Optimization - Single Buffer Strategy

```c
// Example 9 (ST7789) - Double buffer
buf1 = (uint8_t*)lv_malloc(buf_size);
buf2 = (uint8_t*)lv_malloc(buf_size);
lv_display_set_buffers(lcd_disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);

// This project (ST7796) - Single buffer
buf1 = (uint8_t*)lv_malloc(buf_size);
// buf2 commented out to save memory
lv_display_set_buffers(lcd_disp, buf1, NULL, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
```

Benefits:
- **50% less RAM** usage for display buffers
- Essential for larger 480x320 resolution
- Minimal performance impact with partial rendering
- Still provides smooth UI updates

### Dew Point Calculation
Uses the Magnus formula with:
- Water vapor constant: 17.62
- Barometric pressure constant: 243.5
- Formula: `γ = ln(RH/100) + (17.62 × T)/(243.5 + T)`
- Dew point: `DP = 243.5 × γ/(17.62 - γ)`

### Memory Management
- Static buffers for chart data (48 points each for temp/humidity)
- LVGL dynamic memory allocation
- Single display buffer for memory efficiency
- Partial rendering to minimize memory overhead

## 🐛 Troubleshooting

### Common Issues

1. **Display not working**
   - Verify ST7796 controller (not ST7789)
   - Check all connections, especially RST and BL pins
   - Verify SPI clock frequency settings
   - Ensure proper power supply (3.3V)
   - Try the pre-compiled binary from `release/` folder

2. **Display shows wrong colors**
   - Check color inversion setting (should be 0x20 for ST7796)
   - Verify 16-bit RGB565 color mode setting
   - Ensure proper MADCTL rotation value (0xE8 for 270°)

3. **Display resolution incorrect**
   - Confirm SCREEN_WIDTH=480 and SCREEN_HEIGHT=320
   - Verify EEZ Studio project uses 480x320 canvas
   - Check CASET and RASET commands in init sequence

4. **Incorrect sensor readings**
   - Verify I2C pull-up resistors (4.7kΩ recommended)
   - Check sensor address (0x38 default for AHT10)
   - Ensure stable power supply
   - Test I2C communication with `i2cdetect`

5. **UI not responsive / Memory errors**
   - ST7796's larger resolution requires more memory
   - Single buffer mode already reduces usage
   - Check LVGL heap size (64KB default)
   - Monitor free heap with LVGL debug functions
   - Reduce animation complexity if needed

6. **Button issues**
   - Check button pull-up resistors (10kΩ)
   - Verify debounce timing (20ms default)
   - Monitor button GPIO states with printf debugging

## 📝 License

```
MIT License

Copyright (c) 2025 Hardware Innovation Technologies

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## 🏭 About HWIT

Hardware Innovation Technologies (HWIT) specializes in embedded systems development and IoT solutions. This project showcases professional-grade environmental monitoring with attention to both functionality and user interface design on larger display formats.

## 👤 Author

**Juliano Oliveira**
- Company: Hardware Innovation Technologies
- Date: June 10, 2025
- Email: juliano@hwit.com.br
- GitHub: @jrfo-hwit

## 🙏 Acknowledgments

- **Raspberry Pi Foundation** for the Pico SDK and PIO documentation
- **LVGL Team** for the excellent graphics library
- **EEZ Open** team for the visual UI designer
- **Sitronix** for ST7796 and ST7789 controller documentation
- **Community contributors** for testing and feedback

## 📚 Resources

### Documentation
- [Raspberry Pi Pico SDK Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [LVGL v9.4.0 Documentation](https://docs.lvgl.io/9.4/)
- [EEZ Studio Documentation](https://www.envox.hr/eez/studio/studio-introduction.html)
- [ST7796 Datasheet](https://www.displayfuture.com/Display/datasheet/controller/ST7796s.pdf)
- [ST7789 Datasheet](https://www.crystalfontz.com/controllers/Sitronix/ST7789V/)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [AHT10 Sensor Documentation](http://www.aosong.com/en/products-40.html)

### Tutorials & Examples
- [Getting Started with Raspberry Pi Pico](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [PIO Programming Guide](https://datasheets.raspberrypi.com/pico/raspberry-pi-pico-c-sdk.pdf)
- [LVGL Porting Guide](https://docs.lvgl.io/master/porting/index.html)

### Community
- [Raspberry Pi Forums](https://www.raspberrypi.org/forums/)
- [LVGL Forum](https://forum.lvgl.io/)
- [EEZ Studio Forum](https://www.envox.hr/eez/forum/)

### Related Projects
- [Example 9: ST7789 Version](../../../9_st7789_pio_lcd_lvgl_hum_temp/) - Smaller display variant

---

<p align="center">
  Made with ❤️ by Hardware Innovation Technologies
</p>

<p align="center">
  ⭐ Star this repository if you find it helpful!
</p>
