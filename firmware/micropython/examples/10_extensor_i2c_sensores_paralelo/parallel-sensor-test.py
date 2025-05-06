import utime
import sys
from machine import I2C, Timer, Pin, SoftI2C
from ssd1306 import SSD1306_I2C
import gc

import ahtx0
from bmp280_i2c import BMP280I2C
from bh1750 import BH1750
from vl53l0x import VL53L0X
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
        if (sensors == 0x23):
            list_of_sensors.append(BH1750(i2c1))
            addr_of_sensors.append(sensors)
        if (sensors == 0x38):
            list_of_sensors.append(ahtx0.AHT10(i2c1))
            addr_of_sensors.append(sensors)
        if (sensors == 0x68):
            list_of_sensors.append(mpu6500(i2c1)) #mpu6500
            addr_of_sensors.append(sensors)
        if (sensors == 0x76):
            list_of_sensors.append(BMP280I2C(i2c1))
            addr_of_sensors.append(sensors)
        if (sensors == 0x29):
            list_of_sensors.append(VL53L0X(i2c1))
            # the measuting_timing_budget is a value in ms, the longer the budget, the more accurate the reading. 
            budget = list_of_sensors[ids].measurement_timing_budget_us
            print("Budget was:", budget)
            list_of_sensors[ids].set_measurement_timing_budget(40000)
            # Sets the VCSEL (vertical cavity surface emitting laser) pulse period for the 
            # given period type (VL53L0X::VcselPeriodPreRange or VL53L0X::VcselPeriodFinalRange) 
            # to the given value (in PCLKs). Longer periods increase the potential range of the sensor. 
            # Valid values are (even numbers only):
            # tof.set_Vcsel_pulse_period(tof.vcsel_period_type[0], 18)
            list_of_sensors[ids].set_Vcsel_pulse_period(list_of_sensors[ids].vcsel_period_type[0], 12)
            # tof.set_Vcsel_pulse_period(tof.vcsel_period_type[1], 14)
            list_of_sensors[ids].set_Vcsel_pulse_period(list_of_sensors[ids].vcsel_period_type[1], 8)
            addr_of_sensors.append(sensors)            
    return list_of_sensors, addr_of_sensors
        
def get_data(list_of_sensors, addr_of_sensors):
    screen=[[],[], []]
    number_lines = 0
    screen_pos = 0
    for ids, addr_sens in enumerate(addr_of_sensors):
        if (addr_sens == 0x23): # BH1750 Sensor
            num_ln=2
            if (number_lines + num_ln) > 8:
                if(screen_pos + 1 > 1):
                    break
                screen_pos = screen_pos + 1
                number_lines = 0
            screen[screen_pos].append("--BH1750 Sensor-")
            screen[screen_pos].append("Ilum: {:5.2f} ".format(list_of_sensors[ids].measurement))
            number_lines = number_lines + num_ln
            
        if (addr_sens == 0x38): # AHT10 Sensor
            num_ln=3
            if (number_lines + num_ln) > 8:
                if(screen_pos + 1 > 1):
                    break
                screen_pos = screen_pos + 1
                number_lines = 0
            screen[screen_pos].append("--AHT10 Sensor--")
            screen[screen_pos].append("Temp: {:4.2f} C".format(list_of_sensors[ids].temperature))
            screen[screen_pos].append("Humid: {:4.2f} %".format(list_of_sensors[ids].relative_humidity))
            number_lines = number_lines + num_ln
        
        if (addr_sens == 0x76): # BMP280 Sensor
            num_ln=3
            if (number_lines + num_ln) > 8:
                if(screen_pos + 1 > 1):
                    break
                screen_pos = screen_pos + 1
                number_lines = 0
            readout = list_of_sensors[ids].measurements
            screen[screen_pos].append("--BMP280 Sensor-")
            screen[screen_pos].append("Pr: {:5.2f} hPA".format(readout['p']))
            screen[screen_pos].append("Tp: {:5.2f} dgC".format(readout['t']))
            number_lines = number_lines + num_ln
        
        if (addr_sens == 0x29): # VL53L0X Sensor
            num_ln=2
            if (number_lines + num_ln) > 8:
                if(screen_pos + 1 > 1):
                    break
                screen_pos = screen_pos + 1
                number_lines = 0
            screen[screen_pos].append("--VL53L0X Sensor")
            screen[screen_pos].append("dtof: {:} mm".format(list_of_sensors[ids].ping()-50))
            number_lines = number_lines + num_ln
        
        if (addr_sens == 0x68): # mpu6500 Sensor
            num_ln=8
            if (number_lines + num_ln) > 8:
                if(screen_pos + 1 > 1):
                    break
                screen_pos = screen_pos + 1
                number_lines = 0
            readout = list_of_sensors[ids].get_data()
            screen[screen_pos].append("--mpu6500 Sensor")
            screen[screen_pos].append("a_x: {:5.2f} ".format(readout['accel']['x']))
            screen[screen_pos].append("a_y: {:5.2f} ".format(readout['accel']['y']))
            screen[screen_pos].append("a_z: {:5.2f} ".format(readout['accel']['z']))
            screen[screen_pos].append("g_x: {:5.2f} ".format(readout['gyro']['x']))
            screen[screen_pos].append("g_y: {:5.2f} ".format(readout['gyro']['y']))
            screen[screen_pos].append("g_z: {:5.2f} ".format(readout['gyro']['z']))
            screen[screen_pos].append("temp: {:5.2f} ".format(readout['temp']))
            number_lines = number_lines + num_ln
    return screen, (screen_pos+1)
    
# continuous reading
display_cnt=0
list_of_sensors, addr_of_sensors = sensors_list()
while True:
    # Free up hardware resources
    gc.collect()
        
    screens, num_screens = get_data(list_of_sensors, addr_of_sensors)

    if display_cnt < 10:
        #screen 1
        update_oled(screens[0])
        display_cnt = display_cnt + 1
    
    if display_cnt >= 10 and display_cnt < 20:
        if num_screens == 1: # no screen 2
            display_cnt=0
        if num_screens == 2: # 2 screen exists
            #screen 2
            update_oled(screens[1])
            display_cnt = display_cnt + 1
    
    if display_cnt >= 20 and display_cnt < 30:
        if num_screens == 2: # no screen 2
            display_cnt=0
        if num_screens == 3: # 3 screen exists
            #screen 3
            update_oled(screens[2])
            display_cnt = display_cnt + 1
            if display_cnt == 30:
                display_cnt=0
                
    utime.sleep(0.5)
        
sys.exit(0)
