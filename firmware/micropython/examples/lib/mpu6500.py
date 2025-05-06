from machine import Pin, I2C
import utime
import gc

class mpu6500:
    PWR_MGMT_1 = 0x6B
    SMPLRT_DIV = 0x19
    CONFIG = 0x1A
    GYRO_CONFIG = 0x1B
    ACCEL_CONFIG = 0x1C
    TEMP_OUT_H = 0x41
    ACCEL_XOUT_H = 0x3B
    GYRO_XOUT_H = 0x43
        
    def __init__(self, i2c, address=0x68):
        # Free up hardware resources
        gc.collect()
        
        self._i2c = i2c
        self._address = address
        self._i2c.writeto_mem(self._address, self.PWR_MGMT_1, b'\x00')
        utime.sleep_ms(100)
        self._i2c.writeto_mem(self._address, self.SMPLRT_DIV, b'\x07')
        self._i2c.writeto_mem(self._address, self.CONFIG, b'\x00')
        self._i2c.writeto_mem(self._address, self.GYRO_CONFIG, b'\x00')
        self._i2c.writeto_mem(self._address, self.ACCEL_CONFIG, b'\x00')
     
    def read_raw_data(self, addr):
        high = self._i2c.readfrom_mem(self._address, addr, 1)[0]
        low = self._i2c.readfrom_mem(self._address, addr + 1, 1)[0]
        value = high << 8 | low
        if value > 32768:
            value = value - 65536
        return value
     
    def get_data(self):
        temp = ((self.read_raw_data(self.TEMP_OUT_H)- 21) / 333.87) + 21
        accel_x = self.read_raw_data(self.ACCEL_XOUT_H) / 16384.0
        accel_y = self.read_raw_data(self.ACCEL_XOUT_H + 2) / 16384.0
        accel_z = self.read_raw_data(self.ACCEL_XOUT_H + 4) / 16384.0
        gyro_x = self.read_raw_data(self.GYRO_XOUT_H) / 131.0
        gyro_y = self.read_raw_data(self.GYRO_XOUT_H + 2) / 131.0
        gyro_z = self.read_raw_data(self.GYRO_XOUT_H + 4) / 131.0
     
        return {
            'temp': temp,
            'accel': {
                'x': accel_x,
                'y': accel_y,
                'z': accel_z,
            },
            'gyro': {
                'x': gyro_x,
                'y': gyro_y,
                'z': gyro_z,
            }
        }
