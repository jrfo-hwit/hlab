from max30100 import MAX30100
from machine import Pin, I2C, SoftI2C
from ssd1306 import SSD1306_I2C
import time
import utime
import gc

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

def detect_sensor(i2c, oled):
    sensor=i2c.scan()
    found=False
    for i in range(len(sensor)):
        if (sensor[i] == 0x57):
            found=True
    if (found==False):
        print("AHT10 sensors not detected, check cabling\nQuitting.")
        update_oled([
        "----------------",
        "                ",
        " MAX30100 sensor",
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
        " MAX30100 sensor",
        " Detected!!!!   ",
        "                ",
        " Operation OK   ",
        "                ",
        "----------------"
        ])
        utime.sleep(2)
    return

i2c1 = I2C(1, scl=Pin(3), sda=Pin(2), freq=100000)
# Heart Rate and SpO2 Sensor Configuration
sensor = MAX30100(
    i2c1,                        # Use the I2C instance created earlier for communication
    mode=0x03,                  # Set the mode of the MAX30100 sensor (e.g., Heart rate and SpO2 mode)
    sample_rate=100,            # Set the sampling rate for sensor readings (e.g., 100 samples/second)
    led_current_red=11.0,       # Set the LED current for red LED (in mA)
    led_current_ir=11.0         # Set the LED current for IR LED (in mA)
)

# Configuração do OLED
i2c2 = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2c2)

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " BitDogLab      ",
    " MAX30100 sensor",
    " Heart Rate     ",
    " Oximetry       ",
    "                ",
    "----------------"
]
update_oled(messages)

utime.sleep(2)

detect_sensor(i2c1, oled)

# 100 samples are read and used for HR/SpO2 calculation in a single loop
while True:
    gc.collect()

    sensor.read_sensor()  # Read sensor data from the MAX30100 (both IR and Red LED values)
    
    raw_spo2 = sensor.ir         # Store the raw SpO2 value (IR LED data)
    raw_heartrate = sensor.red   # Store the raw heart rate value (Red LED data)

    # Calculate SpO2 and Heart Rate from raw values
    spo2_value = min((raw_spo2 / 100), 100) if raw_spo2 else 0  # Normalize SpO2 value and cap at 100%
    heartrate_value = (raw_heartrate / 200) if raw_heartrate else 0  # Normalize heart rate value

    # Convert rounded values to strings for display or further use
    spo2 = str(round(spo2_value))
    heartrate = str(round(heartrate_value))

    spo2_= "SpO2: {} %".format(spo2)
    heartrate_= "HB: {} bpm".format(heartrate)
    messages = [
        " MAX30100 Sensor",
        "----------------",
        "                ",
        spo2_,
        "                ",
        heartrate_,
        "                "   
    ]
    update_oled(messages)
    # Print SpO2 and Heart Rate values to the console
    print(f"SpO2: {spo2}%\tHeartrate: {heartrate} bpm")
    
    time.sleep(0.15)
