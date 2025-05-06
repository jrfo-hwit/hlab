from machine import I2C, Timer, Pin, SoftI2C, ADC
import time
from ssd1306 import SSD1306_I2C
import gc

# setup softi2c OLED bus
i2cs = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2cs)

# Inicializar ADC para os pinos VRx (GPIO26) e VRy (GPIO27)
adc_vry = ADC(Pin(27))
adc_90_degree = 0
for i in range(10):
    adc_90_degree = adc_90_degree + adc_vry.read_u16()

adc_90_degree = adc_90_degree / 10
adc_0_degree = 280
adc_180_degree = 65100

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " Servo Motor    ",
    " BitDogLab      ",
    " Test           ",
    "                ",
    "                ",
    "----------------"
]
update_oled(messages)

time.sleep(2)

# Initialize PWM on pin 2 for servo control
servo = machine.PWM(machine.Pin(2))
servo.freq(50)  # Set PWM frequency to 50Hz, common for servo motors


def interval_mapping(x, in_min, in_max, out_min, out_max):
    """
    Maps a value from one range to another.
    This function is useful for converting servo angle to pulse width.
    """
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


def servo_write(pin, angle):
    """
    Moves the servo to a specific angle.
    The angle is converted to a suitable duty cycle for the PWM signal.
    """
    pulse_width = interval_mapping(
        angle, 0, 180, 0.5, 2.4
    )  # Map angle to pulse width in ms
    duty = int(
        interval_mapping(pulse_width, 0, 20, 0, 65535)
    )  # Map pulse width to duty cycle
    pin.duty_u16(duty)  # Set PWM duty cycle

ii=0
while True:
    vry_value = adc_vry.read_u16()
#     vry_value = 65535-vry_value # axis reversal
#     adc_90_degree = 65535-adc_90_degree # axis reversal 90 degree calc
    if ii==0:
        print("90 degree = {}".format(adc_90_degree))
        ii=1
    
    adc_x = "adc_x = {}".format(vry_value)
    duty = int(interval_mapping(vry_value, 0, 20, adc_0_degree, adc_180_degree))
    servo.duty_u16(duty)  # Set PWM duty cycle
    
    angle = interval_mapping(vry_value, adc_0_degree, adc_180_degree, 0, 180)
    angle_ = "ang = {:3.2f}".format(angle)
    servo_write(servo, angle)

    # 8 lines x 16 characters
    messages = [
        " Servo Test  ",
        "----------------",
        "                ",
        adc_x,
        angle_,
        "                ",
        "----------------"   
    ]
    update_oled(messages)
    
    gc.collect()
    
#     servo_write(servo, angle) # move to 0 degree
#     time.sleep_ms(500)

#     servo_write(servo, 90) # move to 90 degree
#     time.sleep_ms(2000)
# 
#     servo_write(servo, 0) # move to 0 degree
#     time.sleep_ms(2000)
#     servo_write(servo, 180) # move to 180 degree
#     time.sleep_ms(1000)
