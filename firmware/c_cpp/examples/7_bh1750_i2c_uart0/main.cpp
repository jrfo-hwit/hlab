#include <stdio.h>
#include "pico/stdlib.h"

#include "bh1750.h"

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    bh1750_init();

    printf("BH1750 Luminance Sensor Example\n");

    while (1) {
        uint16_t raw_lux = bh1750_read_lux();
        printf("Luminance: %.2f lux\n", bh1750_raw_to_lux(raw_lux));
        sleep_ms(200); // Allow time for next measurement
    }
}
