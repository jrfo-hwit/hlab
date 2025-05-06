from machine import I2C, Timer, Pin, SoftI2C
from ssd1306 import SSD1306_I2C
import time
from bq25622 import bq25622

#Configuração I2C carregador de bateria
sdaPIN=Pin(0)
sclPIN=Pin(1)
i2c=I2C(0,sda=sdaPIN, scl=sclPIN, freq=100000)
bq = bq25622(i2c)

# Configuração do OLED
i2c2 = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2c2)

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " Power Monitor  ",
    " BitDogLab      ",
    " Chip Carreg.   ",
    " Bateria        ",
    "                ",
    "----------------"
]
update_oled(messages)

time.sleep(3)

while True:
    bq.wdt()

    bq.adc_single_conversion()

    data = bq.ibus_adc()
    str_ibus="ibus: {:5.0f} mA".format(data)
    #print(str_ibus)

    data = bq.ibat_adc()
    str_ibat="ibat: {:5.0f} mA".format(data)
    #print(str_ibat)

    data = bq.vbus_adc()
    str_vbus="vbus: {:5.2f} V".format(data)
    #print(str_vbus)

    data = bq.vbat_adc()
    str_vbat="vbat: {:5.2f} V".format(data)
    #print(str_vbat)

    data = bq.vsys_adc()
    str_vsys="vsys: {:5.2f} V".format(data)
    #print(str_vsys)

    data = bq.dev_rev()
    dev_rev = " BQ25622 rev {}".format(data)

    # 8 lines x 16 characters
    messages = [
        " Power Monitor  ",
        dev_rev,
        "----------------",
        str_vbus,
        str_ibus,
        str_vbat,
        str_ibat,
        str_vsys   
    ]
    update_oled(messages)

    time.sleep(0.1)
