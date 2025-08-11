#include "bh1750.h"

void bh1750_write_command(uint8_t command) {
    uint8_t buf[] = {command};
    i2c_write_blocking(I2C_PORT, BH1750_ADDR, buf, 1, false);
}

uint16_t bh1750_read_lux() {
    uint8_t buf[2];
    i2c_read_blocking(I2C_PORT, BH1750_ADDR, buf, 2, false);
    return (buf[0] << 8 | buf[1]); // Combine bytes and return raw value
}

void bh1750_init() {
    bh1750_write_command(BH1750_POWER_ON);
    bh1750_write_command(MODE::CONTINUOUS_HIGH_RES_MODE);
}

float bh1750_raw_to_lux(uint16_t raw) {
    // Convert raw value to lux
    return static_cast<float>(raw) / 1.2; // Adjust the divisor based on your calibration
}