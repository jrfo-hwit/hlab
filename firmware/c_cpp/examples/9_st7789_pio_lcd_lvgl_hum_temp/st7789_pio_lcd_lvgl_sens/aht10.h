// aht10.h
/**
 * @file
 * @brief Header file for the AHT10 sensor interface.
 *
 * This file defines the functions and constants used to interact with the AHT10 humidity and temperature sensor.
 * It includes function declarations for reading temperature, humidity, and dew point values, as well as necessary
 * constants and types for sensor communication.
 *
 * @author Juliano Oliveira
 * @date 2025-08-10
 */

#ifndef AHT10_H
#define AHT10_H

#include <math.h>
#include "hardware/i2c.h"

// device has default bus address of 0x38
#define ADDR 0x38
#define WATER_VAPOR 17.62f
#define BAROMETRIC_PRESSURE 243.5f

typedef enum {
    eAHT10Address_default = 0x38,
    eAHT10Address_Low     = 0x38,
    eAHT10Address_High    = 0x39,
} HUM_SENSOR_T;

typedef unsigned char Sensor_CMD;

extern Sensor_CMD eSensorCalibrateCmd[3];
extern Sensor_CMD eSensorNormalCmd[3];
extern Sensor_CMD eSensorMeasureCmd[3];
extern Sensor_CMD eSensorResetCmd ;
extern bool    GetRHumidityCmd;
extern bool    GetTempCmd;

#define I2C_PORT i2c1
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

unsigned long readSensor(bool GetDataCmd);
float GetTemperature(void);
float GetHumidity(void);
float GetDewPoint(void);

#endif
