#include <cstring>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define MAX3010X_ADDRESS	0x57

#define I2C_SPEED_STANDARD	100000
#define I2C_SPEED_FAST		400000

#define I2C_BUFFER_LENGTH	32

#define I2C_DELAY        	50000

class MAX3010X {
	public:
		MAX3010X(i2c_inst_t* i2c_type, uint8_t sdata , uint8_t sclk , uint32_t i2cSpeed = I2C_SPEED_STANDARD, uint8_t i2cAddr = MAX3010X_ADDRESS);
		bool begin();

		uint32_t getRed(void); // Returns immediate red value
		uint32_t getIR(void); // Returns immediate IR value
		uint32_t getGreen(void); 
		bool safeCheck(uint8_t maxTimeToCheck); // Given a max amount of time, checks for new data.
		
		// Configuration
		void softReset();
		void shutDown();
		void wakeUp();

		void setLEDMode(uint8_t mode);

		void setADCRange(uint8_t adcRange);
		void setSampleRate(uint8_t sampleRate);
		void setPulseWidth(uint8_t pulseWidth);

		void setPulseAmplitudeRed(uint8_t value);
		void setPulseAmplitudeIR(uint8_t value);
		void setPulseAmplitudeGreen(uint8_t value);
		void setPulseAmplitudeProximity(uint8_t value);

		void setProximityThreshold(uint8_t thresMSB);

		
		// Multi-LED configuration mode
		void enableSlot(uint8_t slotNumber, uint8_t device);
		void disableSlots(void);

		// Data Collection

		// Interrupts
		uint8_t getINT1(void);
		uint8_t getINT2(void);
		void enableAFULL(void);
		void disableAFULL(void);
		void enableDATARDY(void);
		void disableDATARDY(void);
		void enableALCOVF(void);
		void disableALCOVF(void);
		void enablePROXINT(void);
		void disablePROXINT(void);
		void enableDIETEMPRDY(void);
		void disableDIETEMPRDY(void);

		// FIFO Configurations
		void setFIFOAverage(uint8_t samples);
		void enableFIFORollover();
		void disableFIFORollover();
		void setFIFOAlmostFull(uint8_t samples);

		// FIFO Reading
		uint16_t check(void);
		uint8_t available(void);
		void nextSample(void);
		uint32_t getFIFORed(void);
		uint32_t getFIFOIR(void);
		uint32_t getFIFOGreen(void);

		uint8_t getWritePointer(void);
		uint8_t getReadPointer(void);
		void clearFIFO(void);

		// Proximity Mode Interrupt Threshold
		void setPROXINTTHRESH(uint8_t val);

		// Die Temperature
		float readTemperature();
		float readTemperatureF();

		// Detecting ID/Revision
		uint8_t getRevisionID();
		uint8_t readPartID();

		// Setup the sensor with user selectable settings
		void setup(uint8_t powerLevel = 0x1F, uint8_t sampleAverage = 4, uint8_t ledMode = 3, int sampleRate = 400, int pulseWidth = 411, int adcRange = 4096);

		// I2C Communication
		uint8_t readRegister(uint8_t address, uint8_t reg);
		void writeRegister(uint8_t address, uint8_t reg, uint8_t value);
		
	private:
		i2c_inst_t *_i2c;
		uint8_t  _i2caddr;
		uint8_t _SDataPin;
		uint8_t _SClkPin;
		uint32_t _CLKSpeed;

		uint8_t activeLEDs;

		uint8_t revisionID;

		void readRevisionID();

		void bitMask(uint8_t reg, uint8_t mask, uint8_t thing);

		uint8_t readMany[16];

		#define STORAGE_SIZE 4
		typedef struct Record 
		{
			uint32_t red[STORAGE_SIZE];
			uint32_t IR[STORAGE_SIZE];
    		uint32_t green[STORAGE_SIZE];
			uint8_t head;
			uint8_t tail;
		} sense_struct;

		sense_struct sense;
};
