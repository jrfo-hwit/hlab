/**
 * @file st7789_pio.c
 * @brief Implementation of ST7789 display driver using PIO (Programmable I/O) on RP2040.
 *
 * This file contains functions and routines for interfacing with the ST7789
 * display controller using the RP2040's PIO peripheral. It provides low-level
 * routines for initializing the display, sending commands and data, and managing
 * display updates efficiently.
 *
 * @see st7789_pio.h
 * @author Juliano Oliveira
 * @date 2025-06-10
 * @copyright (c) 2025 Hardware Innovation Technologies. All rights reserved.
 * License: MIT License (see LICENSE file for details)
 * 
 */

#include "st7789_lcd_pio.h"

PIO pio = pio0;
uint sm = 0;
/* Example: two dynamically allocated buffers for partial rendering */
uint8_t * buf1 = NULL;
uint8_t * buf2 = NULL;

static inline void lcd_set_dc_cs(bool dc, bool cs) {
    sleep_us(1);
    gpio_put_masked((1u << PIN_DC) | (1u << PIN_CS), !!dc << PIN_DC | !!cs << PIN_CS);
    sleep_us(1);
}

static inline void lcd_write_cmd(PIO pio, uint sm, const uint8_t *cmd, size_t count) {
    st7789_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(0, 0);
    st7789_lcd_put(pio, sm, *cmd++);
    if (count >= 2) {
        st7789_lcd_wait_idle(pio, sm);
        lcd_set_dc_cs(1, 0);
        for (size_t i = 0; i < count - 1; ++i)
            st7789_lcd_put(pio, sm, *cmd++);
    }
    st7789_lcd_wait_idle(pio, sm);
    lcd_set_dc_cs(1, 1);
}

inline void lcd_init(PIO pio, uint sm, const uint8_t *init_seq) {
    const uint8_t *cmd = init_seq;
    while (*cmd) {
        lcd_write_cmd(pio, sm, cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
}

static inline void st7789_start_pixels(PIO pio, uint sm) {
    uint8_t cmd = 0x2c; // RAMWR
    lcd_write_cmd(pio, sm, &cmd, 1);
    lcd_set_dc_cs(1, 0);
}

void lcd_set_window(PIO pio, uint sm, uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1) {
    uint8_t caset[] = {
        0x2A,
        (x0 >> 8) & 0xFF, x0 & 0xFF,
        (x1 >> 8) & 0xFF, x1 & 0xFF
    };
    uint8_t raset[] = {
        0x2B,
        (y0 >> 8) & 0xFF, y0 & 0xFF,
        (y1 >> 8) & 0xFF, y1 & 0xFF
    };

    lcd_write_cmd(pio, sm, caset, sizeof(caset));
    lcd_write_cmd(pio, sm, raset, sizeof(raset));
    st7789_start_pixels(pio, sm);
}