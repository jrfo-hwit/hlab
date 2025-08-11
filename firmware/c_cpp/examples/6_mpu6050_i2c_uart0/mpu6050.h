#ifndef MPU6050_H
#define MPU6050_H

#include "hardware/i2c.h"

#define I2C_PORT i2c1
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

/* Example code to talk to a MPU6050 MEMS accelerometer and gyroscope

   This is taking to simple approach of simply reading registers. It's perfectly
   possible to link up an interrupt line and set things up to read from the
   inbuilt FIFO to make it more useful.

   NOTE: Ensure the device is capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefore I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the
   board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO I2C_SDA_PIN (On Pico this is GP4 (pin 2)) -> SDA on MPU6050 board
   GPIO I2C_SCL_PIN (On Pico this is GP5 (pin 3)) -> SCL on MPU6050 board
   3.3v (pin 36) -> VCC on MPU6050 board
   GND (pin 38)  -> GND on MPU6050 board
*/

// By default these devices  are on bus address 0x68
#define ADDR 0x68

void mpu6050_reset(void);
void mpu6050_read_raw(int16_t accel[3], int16_t gyro[3], int16_t *temp);

#endif // MPU6050_H
