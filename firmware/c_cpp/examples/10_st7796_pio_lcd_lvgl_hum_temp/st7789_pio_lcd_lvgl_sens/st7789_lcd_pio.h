/**
 * @file st7789_pio.h
 * @brief Header file for ST7789 LCD PIO interface
 * @author Juliano Oliveira
 * @date 2025-06-10
 * @copyright (c) 2025 Hardware Innovation Technologies. All rights reserved.
 * License: MIT License (see LICENSE file for details)
 */

#ifndef ST7789_PIO_H
#define ST7789_PIO_H

// Add your declarations here
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "st7789_lcd.pio.h"

// if you want portail:   build ui @ eez studio with 240x320 (update width and height), use  0 or 180 degres rotation only
// if you want landscape: build ui @ eez studio with 320x240 (update width and height), use 90 or 270 degres rotation only
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320
#define ST7789_ROTATION_0 0x00
#define ST7789_ROTATION_90 0x60
#define ST7789_ROTATION_180 0xC0
#define ST7789_ROTATION_270 0xA0
#define ST7789_ROTATION ST7789_ROTATION_0 

// ST7796 320x480 rotation values
#define ST7796_ROTATION_0   0x48   // Portrait (320x480) - MX + BGR
#define ST7796_ROTATION_90  0x28   // Landscape (480x320) - MV + BGR
#define ST7796_ROTATION_180 0x88   // Portrait inverted (320x480) - MY + BGR
#define ST7796_ROTATION_270 0xE8   // Landscape inverted (480x320) - MY + MX + MV + BGR
#define ST7796_ROTATION ST7796_ROTATION_270  // Start with portrait

// Define the pins for the ST7789 LCD @ Pico W
#define PIN_DIN 19
#define PIN_CLK 18
#define PIN_CS 17
#define PIN_DC 4
#define PIN_RESET 20
#define PIN_BL 9

extern PIO pio;
extern uint sm;
/* Example: two dynamically allocated buffers for partial rendering */
extern uint8_t * buf1;
extern uint8_t * buf2;

// Format: cmd length (including cmd byte), post delay in units of 5 ms, then cmd payload
// Note the delays have been shortened a little
static const uint8_t st7789_init_seq[] = {
        1, 20, 0x01,                        // Software reset
        1, 10, 0x11,                        // Exit sleep mode
        2, 2, 0x3a, 0x55,                   // Set colour mode to 16 bit
        2, 0, 0x36, ST7796_ROTATION,        // Set MADCTL: row then column, refresh is bottom to top ????
        5, 0, 0x2a, 0x00, 0x00, SCREEN_WIDTH >> 8, SCREEN_WIDTH & 0xff,   // CASET: column addresses
        5, 0, 0x2b, 0x00, 0x00, SCREEN_HEIGHT >> 8, SCREEN_HEIGHT & 0xff, // RASET: row addresses
        //1, 2, 0x21,                         // Inversion on, then 10 ms delay (supposedly a hack?) for ST7789
        1, 2, 0x20,                         // Inversion off, then 10 ms delay for ST7796
        1, 2, 0x13,                         // Normal display on, then 10 ms delay
        1, 2, 0x29,                         // Main screen turn on, then wait 500 ms
        0                                   // Terminate list
};

void lcd_set_window(PIO pio, uint sm, uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1);
void lcd_init(PIO pio, uint sm, const uint8_t *init_seq);
static void lcd_write_cmd(PIO pio, uint sm, const uint8_t *cmd, size_t count);
static void st7789_start_pixels(PIO pio, uint sm);
static void lcd_set_dc_cs(bool dc, bool cs);
#endif // ST7789_PIO_H