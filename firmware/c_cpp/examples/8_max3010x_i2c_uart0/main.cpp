#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "max3010x.hpp"
#include "algorithm_by_RF.h"
//#include "algorithm.h"

#define MAX3010X_ADDRESS	0x57

#define PIN_WIRE_SDA 2
#define PIN_WIRE_SCL 3
#define I2C_PORT i2c1

MAX3010X heartSensor(I2C_PORT, PIN_WIRE_SDA, PIN_WIRE_SCL, I2C_SPEED_FAST);

#define BUFFER_SIZE 100  // only for algorithm_by_RF.h
uint32_t elapsedTime,timeStart;
uint32_t aun_ir_buffer[BUFFER_SIZE]; //infrared LED sensor data
uint32_t aun_red_buffer[BUFFER_SIZE];  //red LED sensor data
float old_n_spo2;  // Previous SPO2 value
float n_spo2,ratio,correl;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid

int main(void) {

	stdio_init_all();

	busy_wait_ms(500);
	while (heartSensor.begin() != true)
	{
		printf("MAX30102 not connect r fail load calib coeff \r\n");
		busy_wait_ms(500);
	}

	uint8_t powerLevel = 0x1f; //Options: 0=Off to 255=50mA
	uint8_t sampleAverage = 0x04; //Options: 1, 2, 4, 8, 16, 32
	uint8_t ledMode = 0x03; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
	int sampleRate = 400; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
	int pulseWidth = 411; //Options: 69, 118, 215, 411
	int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
	heartSensor.setup(powerLevel, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
    old_n_spo2=0.0;

    printf("Time[s]\tSpO2\tHR\tClock\tRatio\tCorr\tTemp[C]\n");
    timeStart=to_ms_since_boot(get_absolute_time());

	while (1) 
	{
        for(int i=0;i<BUFFER_SIZE;i++)
        { //store the samples in the memory
            aun_red_buffer[i]=heartSensor.getRed();
            aun_ir_buffer[i]=heartSensor.getIR();
        }
        rf_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid, &ratio, &correl); 
        //maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, BUFFER_SIZE, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);
        elapsedTime=to_ms_since_boot(get_absolute_time())-timeStart;
        elapsedTime/=1000; // Time in seconds
        old_n_spo2=n_spo2;

        float temperature = heartSensor.readTemperature();
        
        if(ch_hr_valid && ch_spo2_valid) 
        { 
            printf("%d\t%.1f\t%d\t\t%.2f\t%.2f\t%.2f\n",elapsedTime,n_spo2,n_heart_rate,ratio,correl,temperature);    
        }
        else
        {
            printf("%d\t\t\t\t%.2f\t%.2f\t%.2f <--- not valid\n",elapsedTime,ratio,correl,temperature); 
        }
	}
	return 0;
}
