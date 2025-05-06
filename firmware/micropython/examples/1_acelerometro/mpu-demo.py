import utime
import sys
from machine import I2C, Timer, Pin, SoftI2C
from ssd1306 import SSD1306_I2C
import gc

from mpu6500 import mpu6500
from mpu6050 import mpu6050

# setup the sensor bus i2c1
i2c1 = I2C(1, scl=Pin(3), sda=Pin(2), freq=100000)

# setup softi2c OLED bus
i2cs = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2cs)

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

def sensors_scan(i2c, oled):
    sensor=i2c.scan()
    print(" " + str(len(sensor)) + ": " + str(sensor))
    if (sensor==[]):
        print("Zero sensors detected!!!, check cabling\nQuitting.")
        update_oled([
        "----------------",
        " i2c-1          ",
        " Zero sensors   ",
        " detected!!!!   ",
        " check cabling  ",
        " nQuitting      ",
        "                ",
        "----------------"
        ])
        utime.sleep(2)
        sys.exit(2)
    else:
        addr_line_1 = "";
        addr_line_2 = "";
        for id, sens in enumerate(sensor):
            if id < 4:
                addr_line_1 = addr_line_1 + " " + str(sens)  
            elif id < 8:
                addr_line_2 = addr_line_2 + " " + str(sens) 
        
        if len(addr_line_1)<16:
            for ii in range(1, (len(addr_line_1)-16)):
                addr_line_1 = addr_line_1 + " "
        
        if len(addr_line_2)<16:
            for ii in range(1, (len(addr_line_2)-16)):
                addr_line_2 = addr_line_2 + " " 
        
        update_oled([
        "----------------",
        "                ",
        " i2c-1          ",
        " " + str(len(sensor)) + " Sens. Found",
        " Addresses:     ",
        "" + addr_line_1,
        "" + addr_line_2,
        "----------------"
        ])
        utime.sleep(4)
    return sensor

def sensors_list():
    sensors = sensors_scan(i2c1, oled)
    # Create the sensor object using I2C
    list_of_sensors = []
    addr_of_sensors = []
    for ids, sensors in enumerate(sensors):
        if (sensors == 0x68):
            list_of_sensors.append(mpu6500(i2c1)) #mpu6500
            addr_of_sensors.append(sensors)           
    return list_of_sensors, addr_of_sensors
        
def get_data(list_of_sensors, addr_of_sensors):
    screen=[[]]
    screen_pos = 0
    for ids, addr_sens in enumerate(addr_of_sensors):
        if (addr_sens == 0x68): # mpu6500 Sensor
            readout = list_of_sensors[ids].get_data()
            screen[screen_pos].append("--mpu6500 Sensor")
            screen[screen_pos].append("a_x: {:5.2f} ".format(readout['accel']['x']))
            screen[screen_pos].append("a_y: {:5.2f} ".format(readout['accel']['y']))
            screen[screen_pos].append("a_z: {:5.2f} ".format(readout['accel']['z']))
            screen[screen_pos].append("g_x: {:5.2f} ".format(readout['gyro']['x']))
            screen[screen_pos].append("g_y: {:5.2f} ".format(readout['gyro']['y']))
            screen[screen_pos].append("g_z: {:5.2f} ".format(readout['gyro']['z']))
            screen[screen_pos].append("temp: {:5.2f} ".format(readout['temp']))
    return screen
    
# continuous reading
display_cnt=0
list_of_sensors, addr_of_sensors = sensors_list()
while True:
    # Free up hardware resources
    gc.collect()

    screens = get_data(list_of_sensors, addr_of_sensors)

    update_oled(screens[0])

    utime.sleep(0.5)
        
sys.exit(0)
