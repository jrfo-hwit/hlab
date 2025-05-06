""""
  MicroPythonb Library for the Maxim MAX30100 pulse oximetry system on Pi Pico

  Based on original Python library by mfitzip 
  https: // github.com / mfitzip / MAX30100

  J5  
  MAY 2018
"""
import gc

# ---------------------------------------------------------------
# MAX30100 MicroPython Library for Heart Rate and SpO2 Measurement
# Author: Aman Singh
# Optimized for memory usage 
#
# Version: 1.0.0
# License: MIT License
#
# Credits: This library was inspired by the original MAX30100 drivers,
#          with enhancements for efficient memory management and SoftI2C support.
#          Special thanks to the open-source community for feedback and contributions.
#
# For inquiries or contributions, feel free to open an issue or pull request
# on the official GitHub repository.
# ---------------------------------------------------------------

# Registers for MAX30100
REGISTERS = {
    "INT_STATUS": 0x00,
    "INT_ENABLE": 0x01,
    "FIFO_WR_PTR": 0x02,
    "OVRFLOW_CTR": 0x03,
    "FIFO_RD_PTR": 0x04,
    "FIFO_DATA": 0x05,
    "MODE_CONFIG": 0x06,
    "SPO2_CONFIG": 0x07,
    "LED_CONFIG": 0x09,
    "REV_ID": 0xFE,
    "PART_ID": 0xFF
}

I2C_ADDRESS = 0x57  # I2C address of the MAX30100 device

PULSE_WIDTH = {200: 0, 400: 1, 800: 2, 1600: 3}
SAMPLE_RATE = {50: 0, 100: 1, 167: 2, 200: 3, 400: 4, 600: 5, 800: 6, 1000: 7}
LED_CURRENT = {0: 0, 4.4: 1, 7.6: 2, 11.0: 3, 14.2: 4, 17.4: 5, 20.8: 6, 24.0: 7, 
               27.1: 8, 30.6: 9, 33.8: 10, 37.0: 11, 40.2: 12, 43.6: 13, 46.8: 14, 50.0: 15}

def _get_valid(d, value):
    """Get valid register value or raise KeyError."""
    if value not in d:
        raise KeyError(f"Value {value} not valid, use one of: {', '.join(map(str, d.keys()))}")
    return d[value]

class MAX30100:
    def __init__(self, i2c, mode=0x03, sample_rate=100, led_current_red=11.0, led_current_ir=11.0):
        # Free up hardware resources
        gc.collect()
        
        self.i2c = i2c
        self.buffer_red = 0  # Store only the latest reading
        self.buffer_ir = 0  # Store only the latest reading

        self.set_mode(mode)
        self.set_led_current(led_current_red, led_current_ir)
        self.set_spo_config(sample_rate)

    def i2c_write(self, reg, value):
        """Write a value to a register."""
        self.i2c.writeto_mem(I2C_ADDRESS, reg, bytearray([value]))

    def set_led_current(self, led_current_red=11.0, led_current_ir=11.0):
        """Set LED current for red and infrared LEDs."""
        self.i2c_write(REGISTERS["LED_CONFIG"], (_get_valid(LED_CURRENT, led_current_red) << 4) | _get_valid(LED_CURRENT, led_current_ir))

    def set_mode(self, mode):
        """Set the operation mode (HR or SPO2)."""
        self.i2c_write(REGISTERS["MODE_CONFIG"], self.i2c.readfrom_mem(I2C_ADDRESS, REGISTERS["MODE_CONFIG"], 1)[0] & 0x74 | mode)

    def set_spo_config(self, sample_rate=100, pulse_width=1600):
        """Configure SPO2 settings."""
        self.i2c_write(REGISTERS["SPO2_CONFIG"], (self.i2c.readfrom_mem(I2C_ADDRESS, REGISTERS["SPO2_CONFIG"], 1)[0] & 0xFC) | pulse_width)

    def read_sensor(self):
        """Read the sensor data and update the latest values."""
        bytes = self.i2c.readfrom_mem(I2C_ADDRESS, REGISTERS["FIFO_DATA"], 4)
        self.buffer_ir = bytes[0] << 8 | bytes[1]  # Store only the latest IR value
        self.buffer_red = bytes[2] << 8 | bytes[3]  # Store only the latest red value

    @property
    def red(self):
        """Get the latest red value."""
        return self.buffer_red

    @property
    def ir(self):
        """Get the latest infrared value."""
        return self.buffer_ir