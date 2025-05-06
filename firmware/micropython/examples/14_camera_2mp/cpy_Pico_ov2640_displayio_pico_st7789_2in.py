# SPDX-FileCopyrightText: 2017 Scott Shawcroft, written for Adafruit Industries
# SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
#
# SPDX-License-Identifier: Unlicense
# https://coxxect.blogspot.com/2022/12/ov2640-red-board-without-mclk-on.html
# https://www.youtube.com/watch?v=FxWqNdhCNMU
# https://learn.adafruit.com/circuitpython-libraries-on-micropython-using-the-raspberry-pi-pico/installing-blinka-and-libraries

"""
Capture an image from the camera and display it on a supported LCD.
"""

import time
from displayio import (
    Bitmap,
    Group,
    TileGrid,
    FourWire,
    release_displays,
    ColorConverter,
    Colorspace,
    FourWire,
)

from adafruit_st7789 import ST7789
import board
import busio
import digitalio
import adafruit_ov2640

release_displays()
# Set up the display (You must customize this block for your display!)
"""
spi = busio.SPI(clock=board.GP2, MOSI=board.GP3)
display_bus = FourWire(spi, command=board.GP0, chip_select=board.GP1, reset=None)
display = ST7789(display_bus, width=320, height=240, rotation=270)
"""

# --- Setup st7789 display---
# Define GPIOs for ST7789
tft_SCL = board.GP18
tft_SDA = board.GP19
tft_RES = board.GP20
tft_DC = board.GP16 #########################
tft_CS = board.GP17

# On CircuitPython for Raspberry Pi Pico
# 'module' object has no attribute 'SPI'
#spi = board.SPI()
tft_spi = busio.SPI(clock=tft_SCL, MOSI=tft_SDA)
display_bus = FourWire(tft_spi, command=tft_DC, chip_select=tft_CS, reset=tft_RES)
display = ST7789(display_bus, width=320, height=240, rotation=90)
# -------------


display.auto_refresh = False

# Ensure the camera is shut down, so that it releases the SDA/SCL lines,
# then create the configuration I2C bus
"""
with digitalio.DigitalInOut(board.GP10) as reset:
    reset.switch_to_output(False)
    time.sleep(0.001)
    bus = busio.I2C(board.GP9, board.GP8)

# Set up the camera (you must customize this for your board!)
cam = adafruit_ov2640.OV2640(
    bus,
    data_pins=[
        board.GP12,
        board.GP13,
        board.GP14,
        board.GP15,
        board.GP16,
        board.GP17,
        board.GP18,
        board.GP19,
    ],  # [16]     [org] etc
    clock=board.GP11,  # [15]     [blk]
    vsync=board.GP7,  # [10]     [brn]
    href=board.GP21,  # [27/o14] [red]
    mclk=board.GP20,  # [16/o15]
    shutdown=None,
    reset=board.GP10,
)  # [14]
"""

# ov_SCL   = board.GP1
# ov_SDA   = board.GP0
# ov_RST   = board.GP12
# ov_VSYNC = board.GP14
# ov_HREF  = board.GP13
# ov_DCLK  = board.GP10
# ov_PWDN = board.GP11
# 
# # Pins must be sequential
# ov_D0    = board.GP2
# ov_D1    = board.GP3
# ov_D2    = board.GP4
# ov_D3    = board.GP5
# ov_D4    = board.GP6
# ov_D5    = board.GP7 # GP15
# ov_D6    = board.GP8
# ov_D7    = board.GP9


ov_SDA   = board.GP0
ov_SCL   = board.GP1
ov_DCLK  = board.GP2 #10
ov_PWDN  = board.GP3 #11
ov_RST   = board.GP4 #12
ov_HREF  = board.GP5 #13
ov_VSYNC = board.GP6 #14




# Pins must be sequential
ov_D0    = board.GP8  #2
ov_D1    = board.GP9  #
ov_D2    = board.GP10 #4
ov_D3    = board.GP11 #5
ov_D4    = board.GP12 #6
ov_D5    = board.GP13 #7
ov_D6    = board.GP14 #8
ov_D7    = board.GP15 #9

with digitalio.DigitalInOut(ov_RST) as reset:
    reset.switch_to_output(False)
    time.sleep(0.1)

bus = busio.I2C(scl=ov_SCL, sda=ov_SDA)
cam = adafruit_ov2640.OV2640(
    bus,
    data_pins=[
        ov_D0,
        ov_D1,
        ov_D2,
        ov_D3,
        ov_D4,
        ov_D5,
        ov_D6,
        ov_D7,
    ],
    clock=ov_DCLK,
    vsync=ov_VSYNC,
    href=ov_HREF,
    mclk=None,
    size=adafruit_ov2640.OV2640_SIZE_240X240,#OV2640_SIZE_QQVGA,
)

width = display.width
height = display.height

cam.size = adafruit_ov2640.OV2640_SIZE_240X240#OV2640_SIZE_QQVGA
#cam.test_pattern = True
bitmap = Bitmap(cam.width, cam.height, 65536)

print(width, height, cam.width, cam.height)
if bitmap is None:
    raise SystemExit("Could not allocate a bitmap")

g = Group(scale=1, x=(width - cam.width) // 2, y=(height - cam.height) // 2)
tg = TileGrid(
    bitmap, pixel_shader=ColorConverter(input_colorspace=Colorspace.RGB565_SWAPPED)#BGR565_SWAPPED)
)
g.append(tg)
display.root_group = g

display.auto_refresh = False
while True:
    cam.capture(bitmap)
    bitmap.dirty()
    display.refresh(minimum_frames_per_second=0)


