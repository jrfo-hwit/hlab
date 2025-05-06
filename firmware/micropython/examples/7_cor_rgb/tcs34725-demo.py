import utime
import sys
from machine import I2C, Timer, Pin, SoftI2C
from ssd1306 import SSD1306_I2C
from tcs34725 import TCS34725

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

def detect_sensor(i2c, oled):
    sensor=i2c.scan()
    found=False
    for i in range(len(sensor)):
        if (sensor[i] == 0x29):
            found=True
    if (found==False):
        print("AHT10 sensors not detected, check cabling\nQuitting.")
        update_oled([
        "----------------",
        "                ",
        " TCS34725 sensor",
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
        " TCS34725 sensor",
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
    " TCS34725 sensor",
    " RGB Color      ",
    "                ",
    "                ",
    "----------------"
]
update_oled(messages)

utime.sleep(2)

detect_sensor(i2c, oled)

# Create the sensor object using I2C
tcs34725 = TCS34725(i2c)
tcs34725.gain = 1 #TCSGAIN_LOW
tcs34725.integ = 192 #TCSINTEG_HIGH
tcs34725.autogain = True

# continuous reading
while True:
    colors = tcs34725.colors
    print(colors)
    #print(f'Temperature: {sensor.temperature} °C')
    #print(f'Relative Humidity: {sensor.relative_humidity}%')
    #print('')
    clear= "Clr: {}".format(colors[0]>>8) # acima de 40 está identificando alguma cor
    red= "R  : {}".format(colors[1]>>8)
    green= "G  : {}".format(colors[2]>>8)
    blue= "B  : {}".format(colors[3]>>8)
    # 8 lines x 16 characters
    messages = [
        " TCS34725 Sensor",
        "----------------",
        "     0-255      ",
        clear,
        red,
        green,
        blue   
    ]
    update_oled(messages)
    
    utime.sleep(0.2)

sys.exit(0)