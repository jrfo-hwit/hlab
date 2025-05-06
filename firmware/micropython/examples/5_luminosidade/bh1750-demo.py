from machine import Pin, I2C, SoftI2C
from utime import sleep
from ssd1306 import SSD1306_I2C
from bh1750 import BH1750

i2c1_sda = Pin(2)
i2c1_scl = Pin(3)
i2c1 = I2C(1, sda=i2c1_sda, scl=i2c1_scl)

bh1750 = BH1750(i2c1)

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()
    
def detect_sensor(i2c, oled):
    sensor=i2c.scan()
    found=False
    for i in range(len(sensor)):
        if (sensor[i] == 0x23):
            found=True
    if (found==False):
        print("AHT10 sensors not detected, check cabling\nQuitting.")
        update_oled([
        "----------------",
        "                ",
        " BH1750         ",
        " Illumination   ",
        " Sens. not found",
        " nQuitting      ",
        "                ",
        "----------------"
        ])
        sleep(2)
        sys.exit(2)
    else:
        update_oled([
        "----------------",
        "                ",
        " BH1750         ",
        " Illumination   ",
        " Sens found!!!  ",
        " OK!!!!         ",
        "                ",
        "----------------"
        ])
        sleep(2)
    return

# Configuração do OLED
i2c2 = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2c2)

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " BitDogLab      ",
    " BH1750         ",
    " Illumination   ",
    " Sensor         ",
    "                ",
    "----------------"
]
update_oled(messages)

sleep(2)

detect_sensor(i2c1, oled)

while True:
    #print(bh1750.measurement)
    Ilum= "Ilum: {:5.2f} ".format(bh1750.measurement)
    # 8 lines x 16 characters
    messages = [
        " AHT10 Sensor   ",
        "----------------",
        "                ",
        Ilum,
        "                ",
        "                ",
        "                "   
    ]
    update_oled(messages)
    
    sleep(1)