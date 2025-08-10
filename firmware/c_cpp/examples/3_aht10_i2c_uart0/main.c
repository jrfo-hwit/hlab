// main.c
/**
 * @file
 * @brief Main application file for reading temperature and humidity from the AHT10 sensor.
 *
 * This file initializes the I2C communication, reads data from the AHT10 sensor,
 * and prints the temperature, humidity, and dew point values to the console.
 *
 * @author Juliano Oliveira
 * @date 2025-08-10
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "hardware/i2c.h"

#include "aht10.h"

int main() {
    stdio_init_all();
    
    // Initialise the Wi-Fi chip
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }

    // Example to turn on the Pico W LED
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    printf("Hello, AHT10! Reading temperaure and humidity values from sensor...\n");

    // I2C is "open drain", pull ups to keep signal high when no data is being sent
    i2c_init(I2C_PORT, 100 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    float temperature;
    float humidity;
    float dew;

    sleep_ms(250); // sleep so that data polling and register update don't collide
    while (1) {
        temperature = GetTemperature();
        humidity= GetHumidity();
        dew = GetDewPoint();
        printf("Hum. = %.2f %%\n", humidity);
        printf("Temp. = %.2f C\n", temperature);
        printf("Dew = %.2f C\n", dew);
        // poll every 500ms
        sleep_ms(500);
    }

}
