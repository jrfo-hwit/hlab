import utime
import sys
from machine import I2C, Timer, Pin, SoftI2C
from ssd1306 import SSD1306_I2C
import ahtx0

#---- CONSTANTS --------------------------------------------------------------

TP_ADDR = 0x38 
TP_INIT = 0xE1
TP_READ = 0xAC

# float precision
PRECISION = 2
#-----------------------------------------------------------------------------

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

def detect_sensor(i2c, oled):
    sensor=i2c.scan()
    found=False
    for i in range(len(sensor)):
        if (sensor[i] == 56):
            found=True
    if (found==False):
        print("AHT10 sensors not detected, check cabling\nQuitting.")
        update_oled([
        "----------------",
        "                ",
        " AHT10 sensors  ",
        " not detected   ",
        " check cabling  ",
        " nQuitting      ",
        "                ",
        "----------------"
        ])
        utime.sleep(2)
        sys.exit(2)
    else:
        update_oled([
        "----------------",
        "                ",
        " AHT10 sensors  ",
        " Detected!!!!   ",
        "                ",
        " Operation OK   ",
        "                ",
        "----------------"
        ])
        utime.sleep(2)
    return

# setup the bus
i2c = I2C(1, scl=machine.Pin(3), sda=machine.Pin(2), freq=100000)
# Configuração do OLED
i2c2 = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2c2)

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " BitDogLab      ",
    " AHT10 sensor   ",
    " temperature    ",
    " humidity       ",
    "                ",
    "----------------"
]
update_oled(messages)

utime.sleep(2)

detect_sensor(i2c, oled)

# Create the sensor object using I2C
sensor = ahtx0.AHT10(i2c)

# continuous reading
while True:
    
    tem= "Temp: {:4.2f} C".format(sensor.temperature)
    hum= "Humid: {:4.2f} %".format(sensor.relative_humidity)

    #print('------------------------------------')
    #print(f'Temperature: {sensor.temperature} °C')
    #print(f'Relative Humidity: {sensor.relative_humidity}%')
    #print('')
    
    # 8 lines x 16 characters
    messages = [
        " AHT10 Sensor   ",
        "----------------",
        "                ",
        tem,
        "                ",
        hum,
        "                "   
    ]
    update_oled(messages)
    
    utime.sleep(0.2)

sys.exit(0)