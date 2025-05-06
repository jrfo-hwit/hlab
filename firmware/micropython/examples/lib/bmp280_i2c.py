# https://github.com/flrrth/pico-bmp280

from bmp280 import BMP280, BMP280Configuration
import gc

class BMP280I2C(BMP280):
    """The I2C implementation for the BMP280."""
    
    bmp280_addr = 0x76
    
    def __init__(self, i2c, address=bmp280_addr, configuration=BMP280Configuration()):
        # Free up hardware resources
        gc.collect()
        self._address = address
        self._i2c = i2c
        super().__init__(configuration)
        self._read_compensation_parameters()
        
    def _write(self, register, txdata):
        self._i2c.writeto_mem(self._address, register, txdata)
        
    def _read(self, register, nbytes):        
        return self._i2c.readfrom_mem(self._address, register, nbytes)
    
    def _sensor_address(self):
        return bmp280_addr
        
