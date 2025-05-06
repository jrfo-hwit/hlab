from machine import SPI, Pin, SoftI2C
import sdcard, os
from ssd1306 import SSD1306_I2C
import time

# setup softi2c OLED bus
i2cs = SoftI2C(scl=Pin(15), sda=Pin(14))
oled = SSD1306_I2C(128, 64, i2cs)

# Constants
SPI_BUS = 0
SCK_PIN = 18
MOSI_PIN = 19
MISO_PIN = 16
CS_PIN = 17
SD_MOUNT_PATH = '/sd'
FILE_PATH = 'sd/sd_file.txt'

def update_oled(lines):
    oled.fill(0)
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 8)
    oled.show()

# 8 lines x 16 characters
messages = [
    "----------------",
    "                ",
    " SDCARD         ",
    " BitDogLab      ",
    " Test           ",
    "                ",
    "                ",
    "----------------"
]
update_oled(messages)

time.sleep(2)

try:
    # Init SPI communication
    spi = SPI(SPI_BUS,sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), miso=Pin(MISO_PIN))
    cs = Pin(CS_PIN)
    sd = sdcard.SDCard(spi, cs)
    # Mount microSD card
    os.mount(sd, SD_MOUNT_PATH)
    print('before file creation')
    # List files on the microSD card
    print(os.listdir(SD_MOUNT_PATH))
    
    # Create new file on the microSD card
    with open(FILE_PATH, "w") as file:  # ‘w’: writing mode — allows writing to a file, overwriting existing content;
        # Write to the file				# ‘a’: appending mode — appending new data to the end of an existing file.
        file.write("Hello SDCARD\n")
    
    print('after file creation')
    # Check that the file was created:
    print(os.listdir(SD_MOUNT_PATH))
    
    # Open the file in reading mode
    with open(FILE_PATH, "r") as file: # ‘r’: reading mode — enables reading from an existing file;
        # read the file content
        content = file.read()
        print("File content:", content)  
    
        # 8 lines x 16 characters
    messages = [
        " SDCARD Test    ",
        "----------------",
        " File           ",
        FILE_PATH,
        " Content        ",
        content,
        "----------------"   
    ]
    update_oled(messages)
    
except Exception as e:
    print('An error occurred:', e)