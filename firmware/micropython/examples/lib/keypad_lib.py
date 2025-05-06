"""
This MicroPython library is designed for any hardware plaform that supports MicroPython such as Raspberry Pi Pico, ESP32, Micro:bit... to make it easy to use with keypad 3x4, keypad 4x4 

It is created by DIYables to work with DIYables products, but also work with products from other brands. Please consider purchasing products from [DIYables Store on Amazon](https://amazon.com/diyables) from to support our work.

Product Link:
- [Keypad 3x4](https://diyables.io/products/keypad-3x4)
- [Keypad 4x4](https://diyables.io/products/keypad-4x4)


Copyright (c) 2024, DIYables.io. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

- Neither the name of the DIYables.io nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY DIYABLES.IO "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL DIYABLES.IO BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
"""

from machine import Pin, Timer
import time

class Keypad:
    def __init__(self, keymap, row_pins, column_pins, num_rows, num_cols):
        self._keymap = keymap
        self._row_pins = [Pin(pin, Pin.IN, Pin.PULL_UP) for pin in row_pins]
        self._column_pins = [Pin(pin, Pin.OUT) for pin in column_pins]
        self._num_rows = num_rows
        self._num_cols = num_cols
        self._prev_key = None
        self._debounce_time = 400  # default value is 300ms
        self._prev_time = 0
        
        # Set column pins to high
        for col_pin in self._column_pins:
            col_pin.value(1)
    
    def get_key(self):
        for col_index, col_pin in enumerate(self._column_pins):
            col_pin.value(0)  # Set current column to low
            for row_index, row_pin in enumerate(self._row_pins):
                if not row_pin.value():  # Active low
                    key = self._keymap[row_index * self._num_cols + col_index]
                    current_time = time.ticks_ms()
                    if self._prev_key != key or (self._prev_key == key and time.ticks_diff(current_time, self._prev_time) > self._debounce_time):
                        self._prev_key = key
                        self._prev_time = current_time
                        col_pin.value(1)  # Reset column pin
                        return key
            col_pin.value(1)  # Reset column pin
        return None
    
    def get_key_blocking(self):
        while True:
            for col_index, col_pin in enumerate(self._column_pins):
                col_pin.value(0)  # Set current column to low
                for row_index, row_pin in enumerate(self._row_pins):
                    if not row_pin.value():  # Active low
                        key = self._keymap[row_index * self._num_cols + col_index]
                        current_time = time.ticks_ms()
                        if self._prev_key != key or (self._prev_key == key and time.ticks_diff(current_time, self._prev_time) > self._debounce_time):
                            self._prev_key = key
                            self._prev_time = current_time
                            col_pin.value(1)  # Reset column pin
                            return key
                col_pin.value(1)  # Reset column pin

    def set_debounce_time(self, time_ms):
        self._debounce_time = time_ms

