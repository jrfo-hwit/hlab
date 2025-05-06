from machine import Pin, SPI, SoftI2C
from ad5592 import AD5592R
from ssd1306 import SSD1306_I2C
import time

# setup softi2c OLED bus
i2cs = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2cs)

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()
    
# === Usage Example ===

spi = SPI(0, baudrate=1000000, polarity=1, phase=0, sck=Pin(18), mosi=Pin(19), miso=Pin(16))
cs = Pin(17, Pin.OUT) #direto
#cs = Pin(4, Pin.OUT) #via idc extender
cs.init(Pin.OUT, value=1) # Initialise CS high, ie no transmission

#rst = Pin(20, Pin.OUT)
#rst.init(Pin.OUT, value=1) # Initialise CS high, ie no transmission

ad5592 = AD5592R(spi, cs)#, rst) # rst not needed for idc extender

# Configure CH4 as DAC, CH5 as ADC, CH0 & CH1 as GPIO
dac_channels=[4]
adc_channels=[5]
gpio_in_channels=[0]
gpio_out_channels=[1]
ad5592.init_chip(dac_channels, adc_channels, gpio_in_channels, gpio_out_channels, internal_reference=1)

# Example loop
count = 2
numbers = [0, 1023, 2047, 3071, 4095]
logic_=1

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " ADC/DAC        ",
    " BitDogLab      ",
    " Test           ",
    "                ",
    "                ",
    "----------------"
]
update_oled(messages)

time.sleep(2)

# Set DAC and GPIO, read ADC and GPIO
ad5592.set_dac(dac_channels[0], numbers[count])
dac_="DAC_{}: {}".format(dac_channels[0], numbers[count])

adc_val = ad5592.read_adc(adc_channels[0])
adc_="ADC_{}: {}".format(adc_channels[0], adc_val)

ad5592.gpio_set(gpio_out_channels[0], logic_)
io_o_="IOo_{}: {}".format(gpio_out_channels[0], logic_)

io_get = ad5592.gpio_get(gpio_in_channels[0])
io_i_="IOi_{}: {}".format(gpio_in_channels[0], io_get)
    
messages = [
    " ADC/DAC Test   ",
    "----------------",
    "                ",
    dac_,
    adc_,
    io_i_,
    io_o_,
    "                "  
]
update_oled(messages)
time.sleep(0.5)

# loop adc with dac and gpio out with gpio in
while True:
    count = count + 1
    if count == 5:
        count = 0
        
    # Set DAC
    ad5592.set_dac(dac_channels[0], numbers[count])
    dac_="DAC_{}: {}".format(dac_channels[0], numbers[count])

    messages = [
        " ADC/DAC Test   ",
        "----------------",
        "                ",
        dac_,
        adc_,
        io_o_,
        io_i_,
        "                "  
    ]
    update_oled(messages)
    
    time.sleep(0.5)

    # read ADC
    adc_val = ad5592.read_adc(adc_channels[0])
    adc_="ADC_{}: {}".format(adc_channels[0], adc_val)

    messages = [
        " ADC/DAC Test   ",
        "----------------",
        "                ",
        dac_,
        adc_,
        io_o_,
        io_i_,
        "                "  
    ]
    update_oled(messages)
    
    time.sleep(0.5)
    
    # Set GPIO   
    ad5592.gpio_set(gpio_out_channels[0], logic_)
    io_o_="IOo_{}: {}".format(gpio_out_channels[0], logic_)

    messages = [
        " ADC/DAC Test   ",
        "----------------",
        "                ",
        dac_,
        adc_,
        io_o_,
        io_i_,
        "                "  
    ]
    update_oled(messages)
    
    logic_= (~ logic_) & 0x01
    time.sleep(0.5)
    
    # read GPIO
    io_get = ad5592.gpio_get(gpio_in_channels[0])
    io_i_="IOi_{}: {}".format(gpio_in_channels[0], io_get)

    messages = [
        " ADC/DAC Test   ",
        "----------------",
        "                ",
        dac_,
        adc_,
        io_o_,
        io_i_,
        "                "  
    ]
    update_oled(messages)
    time.sleep(0.5)
    
    #ad5592.print_all_registers() # resposta deslocada
