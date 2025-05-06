from machine import Pin, I2C, SoftI2C
from utime import sleep
from ssd1306 import SSD1306_I2C
from bmp280_i2c import BMP280I2C

i2c1_sda = Pin(2)
i2c1_scl = Pin(3)
i2c1 = I2C(1, sda=i2c1_sda, scl=i2c1_scl, freq=400000)

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()
    
def detect_sensor(i2c, oled):
    sensor=i2c.scan()
    found=False
    for i in range(len(sensor)):
        if (sensor[i] == 0x76):
            found=True
    if (found==False):
        print("BMP280 sensors not detected, check cabling\nQuitting.")
        update_oled([
        "----------------",
        "                ",
        " BMP280         ",
        " Pressure & Temp",
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
        " BMP280         ",
        " Pressure & Temp",
        " Sensor found!!! ",
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
    " BMP280         ",
    " Pressure &     ",
    " Temperature    ",
    " Sensor         ",
    "----------------"
]
update_oled(messages)

sleep(2)

detect_sensor(i2c1, oled)

bmp280_i2c = BMP280I2C(i2c1)  # address may be different

while True:
    readout = bmp280_i2c.measurements
    #print(f"Temperature: {readout['t']} °C, pressure: {readout['p']} hPa.")
    pressure= "Pr: {:5.2f} hPA".format(readout['p'])
    temperature= "Tp: {:5.2f} dgC".format(readout['t'])
    
    #Barometric Formula (Simplified):
    # h = 44330 * (1 - (P/P0)^(1/5.255)):
    # h = altitude (m)
    # P = measured pressure (Pa)
    # P0 = reference pressure at sea level (typically 1013.25 hPa or 1013.25 mbar or 101325 Pa)
    altitude = 44330 * (1 - ((readout['p']*100) / 101325)**(1/5.255))
    altitude_= "Alt: {:5.2f} m".format(altitude)
    # 8 lines x 16 characters
    messages = [
        " BMP280 Sensor  ",
        "----------------",
        "                ",
        pressure,
        "                ",
        temperature,
        "                ",
        altitude_   
    ]
    update_oled(messages)
    
    sleep(0.2)