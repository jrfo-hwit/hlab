#ifndef BH1750_H
#define BH1750_H

#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA 2
#define I2C_SCL 3

#define BH1750_ADDR 0x23 // Or 0x5C depending on ADDR pin
#define BH1750_ALT_ADDRESS 0x5C  // I2C address with ADDR pin high
#define BH1750_POWER_ON 0x01

#define _BH1750_MTREG_MIN 31
#define _BH1750_MTREG_MAX 254
#define _BH1750_DEFAULT_MTREG 69

enum MODE {
   // same as Power Down
   UNCONFIGURED = 0,
   // Measurement at 1 lux resolution. Measurement time is approx 120ms.
   CONTINUOUS_HIGH_RES_MODE = 0x10,
   // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
   CONTINUOUS_HIGH_RES_MODE_2 = 0x11,
   // Measurement at 4 lux resolution. Measurement time is approx 16ms.
   CONTINUOUS_LOW_RES_MODE = 0x13,
   // Measurement at 1 lux resolution. Measurement time is approx 120ms.
   ONE_TIME_HIGH_RES_MODE = 0x20,
   // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
   ONE_TIME_HIGH_RES_MODE_2 = 0x21,
   // Measurement at 4 lux resolution. Measurement time is approx 16ms.
   ONE_TIME_LOW_RES_MODE = 0x23
};

void bh1750_write_command(uint8_t command);
uint16_t bh1750_read_lux();
void bh1750_init();
float bh1750_raw_to_lux(uint16_t raw);

#endif // BH1750_H