#include "max3010x.hpp"

// Status Registers
static const uint8_t REG_INTSTAT1 =				0x00;
static const uint8_t REG_INTSTAT2 =				0x01;
static const uint8_t REG_INTENABLE1 =			0x02;
static const uint8_t REG_INTENABLE2 =			0x03;

// FIFO Registers
static const uint8_t REG_FIFOWRITEPTR =			0x04;
static const uint8_t REG_FIFOOVERFLOW =			0x05;
static const uint8_t REG_FIFOREADPTR =			0x06;
static const uint8_t REG_FIFODATA =				0x07;

// Configuration Registers
static const uint8_t REG_FIFOCONFIG =			0x08;
static const uint8_t REG_MODECONFIG =			0x09;
static const uint8_t REG_PARTICLECONFIG =		0x0A;
static const uint8_t REG_LED1_PULSEAMP =		0x0C;
static const uint8_t REG_LED2_PULSEAMP =		0x0D;
static const uint8_t REG_LED3_PULSEAMP =		0x0E;
static const uint8_t REG_LED_PROX_AMP =			0x10;
static const uint8_t REG_MULTILEDCONFIG1 =		0x11;
static const uint8_t REG_MULTILEDCONFIG2 =		0x12;

// Die Temperature Registers
static const uint8_t REG_DIETEMPINT =			0x1F;
static const uint8_t REG_DIETEMPFRAC =			0x20;
static const uint8_t REG_DIETEMPCONFIG =		0x21;

// Proximity Function Registers
static const uint8_t REG_PROXINTTHRESH =		0x30;

// IDs
static const uint8_t REG_REVISIONID = 			0xFE;
static const uint8_t REG_PARTID =				0xFF;
static const uint8_t MAX3010X_EXPECTEDPARTID =	0x15;

// Interrupt Configuration
static const uint8_t MASK_INT_A_FULL =			(uint8_t)~0b10000000;
static const uint8_t INT_A_FULL_ENABLE =		0x80;
static const uint8_t INT_A_FULL_DISABLE =		0x00;

static const uint8_t MASK_INT_DATA_RDY =		(uint8_t)~0b01000000;
static const uint8_t INT_DATA_RDY_ENABLE =		0x40;
static const uint8_t INT_DATA_RDY_DISABLE =		0x00;

static const uint8_t MASK_INT_ALC_OVF =			(uint8_t)~0b00100000;
static const uint8_t INT_ALC_OVF_ENABLE =		0x20;
static const uint8_t INT_ALC_OVF_DISABLE =		0x00;

static const uint8_t MASK_INT_PROX_INT =		(uint8_t)~0b00010000;
static const uint8_t INT_PROX_INT_ENABLE =		0x10;
static const uint8_t INT_PROX_INT_DISABLE =		0x00;

static const uint8_t MASK_INT_DIE_TEMP_RDY =	(uint8_t)~0b00000010;
static const uint8_t INT_DIE_TEMP_RDY_ENABLE =	0x02;
static const uint8_t INT_DIE_TEMP_RDY_DISABLE =	0x00;

static const uint8_t MASK_SAMPLEAVG =			(uint8_t)~0b11100000;
static const uint8_t SAMPLEAVG_1 =				0x00;
static const uint8_t SAMPLEAVG_2 =				0x20;
static const uint8_t SAMPLEAVG_4 =				0x40;
static const uint8_t SAMPLEAVG_8 =				0x60;
static const uint8_t SAMPLEAVG_16 =				0x80;
static const uint8_t SAMPLEAVG_32 =				0xA0;

static const uint8_t MASK_ROLLOVER =			0xEF;
static const uint8_t ROLLOVER_ENABLE =			0x10;
static const uint8_t ROLLOVER_DISABLE =			0x00;

static const uint8_t MASK_A_FULL =				0xF0;

// Mode configuration commands
static const uint8_t MASK_SHUTDOWN =			0x7f;
static const uint8_t SHUTDOWN =					0x80;
static const uint8_t WAKEUP =					0x00;

static const uint8_t MASK_RESET =				0xBF;
static const uint8_t RESET =					0X40;
/// IR led mode
static const uint8_t MASK_LEDMODE =				0xF8;
static const uint8_t LEDMODE_REDONLY =			0x02;
static const uint8_t LEDMODE_REDIRONLY =		0x03;
static const uint8_t LEDMODE_MULTILED =			0x07;

// Particle sensing configuration commands
static const uint8_t MASK_ADCRANGE =			0x9F;
static const uint8_t ADCRANGE_2048 =			0x00;
static const uint8_t ADCRANGE_4096 =			0x20;
static const uint8_t ADCRANGE_8192 =			0x40;
static const uint8_t ADCRANGE_16384 =			0x60;

static const uint8_t MASK_SAMPLERATE =			0xE3;
static const uint8_t SAMPLERATE_50 =			0x00;
static const uint8_t SAMPLERATE_100 =			0x04;
static const uint8_t SAMPLERATE_200 =			0x08;
static const uint8_t SAMPLERATE_400 =			0x0C;
static const uint8_t SAMPLERATE_800 =			0x10;
static const uint8_t SAMPLERATE_1000 =			0x14;
static const uint8_t SAMPLERATE_1600 =			0x18;
static const uint8_t SAMPLERATE_3200 =			0x1C;

static const uint8_t MASK_PULSEWIDTH =			0xFC;
static const uint8_t PULSEWIDTH_69 =			0x00;
static const uint8_t PULSEWIDTH_118 =			0x01;
static const uint8_t PULSEWIDTH_215 =			0x02;
static const uint8_t PULSEWIDTH_411 =			0x03;

// Multi-LED Mode Configuration
static const uint8_t MASK_SLOT1 =				0xF8;
static const uint8_t MASK_SLOT2 =				0x8F;
static const uint8_t MASK_SLOT3 =				0xF8;
static const uint8_t MASK_SLOT4 =				0x8F;

static const uint8_t SLOT_NONE =				0x00;
static const uint8_t SLOT_RED_LED =				0x01;
static const uint8_t SLOT_IR_LED =				0x02;
static const uint8_t SLOT_GREEN_LED =			0x03;
static const uint8_t SLOT_NONE_PILOT =			0x04;
static const uint8_t SLOT_RED_PILOT =			0x05;
static const uint8_t SLOT_IR_PILOT =			0x06;
static const uint8_t SLOT_GREEN_PILOT =			0x07;

MAX3010X::MAX3010X(i2c_inst_t* i2c_type, uint8_t SDApin , uint8_t SCLKpin, uint32_t i2cSpeed, uint8_t i2cAddr) {
	// Constructor
	_i2caddr = i2cAddr;
	_i2c = i2c_type; 
    _SClkPin = SCLKpin;
    _SDataPin = SDApin;
    _CLKSpeed = i2cSpeed;
}

/**
 * Initializes sensor.
 * Returns negative number on failure.
 * Returns sensor revision on success.
 */
bool MAX3010X::begin() {

	i2c_init(_i2c, _CLKSpeed);
	gpio_set_function(_SDataPin, GPIO_FUNC_I2C);
    gpio_set_function(_SClkPin, GPIO_FUNC_I2C);
	gpio_pull_up(_SDataPin);
    gpio_pull_up(_SClkPin);
	
	if (readPartID() != MAX3010X_EXPECTEDPARTID)
	{
		return false;
	}
	
	readRevisionID();
	return true;
}


// INterrupt configuration //

uint8_t MAX3010X::getINT1(void) {
	return readRegister(_i2caddr, REG_INTSTAT1);
	// return (i2c_smbus_read_byte_data(_i2c, REG_INTSTAT1));
}
uint8_t MAX3010X::getINT2(void) {
	return readRegister(_i2caddr, REG_INTSTAT2);
	// return (i2c_smbus_read_byte_data(_i2c, REG_INTSTAT2));
}

void MAX3010X::enableAFULL(void) {
	bitMask(REG_INTENABLE1, MASK_INT_A_FULL, INT_A_FULL_ENABLE);
}
void MAX3010X::disableAFULL(void) {
	bitMask(REG_INTENABLE1, MASK_INT_A_FULL, INT_A_FULL_DISABLE);
}

void MAX3010X::enableDATARDY(void) {
	bitMask(REG_INTENABLE1, MASK_INT_DATA_RDY, INT_DATA_RDY_ENABLE);
}
void MAX3010X::disableDATARDY(void) {
	bitMask(REG_INTENABLE1, MASK_INT_DATA_RDY, INT_DATA_RDY_DISABLE);
}

void MAX3010X::enableALCOVF(void) {
	bitMask(REG_INTENABLE1, MASK_INT_ALC_OVF, INT_ALC_OVF_ENABLE);
}
void MAX3010X::disableALCOVF(void) {
	bitMask(REG_INTENABLE1, MASK_INT_ALC_OVF, INT_ALC_OVF_DISABLE);
}

void MAX3010X::enablePROXINT(void) {
	bitMask(REG_INTENABLE1, MASK_INT_PROX_INT, INT_PROX_INT_ENABLE);
}
void MAX3010X::disablePROXINT(void) {
	bitMask(REG_INTENABLE1, MASK_INT_PROX_INT, INT_PROX_INT_DISABLE);
}

void MAX3010X::enableDIETEMPRDY(void) {
	bitMask(REG_INTENABLE2, MASK_INT_DIE_TEMP_RDY, INT_DIE_TEMP_RDY_ENABLE);
}
void MAX3010X::disableDIETEMPRDY(void) {
	bitMask(REG_INTENABLE2, MASK_INT_DIE_TEMP_RDY, INT_DIE_TEMP_RDY_DISABLE);
}


// Mode configuration //

/**
 * Pull sensor out of low power mode.
 */
void MAX3010X::wakeUp(void) {
	bitMask(REG_MODECONFIG, MASK_SHUTDOWN, WAKEUP);
}

/**
 * Put sensor into low power mode.
 * During this mode the sensor will continue to respond to I2C commands
 * but will not update or take new readings, such as temperature.
 */
void MAX3010X::shutDown(void) {
	bitMask(REG_MODECONFIG, MASK_SHUTDOWN, SHUTDOWN);
}

/**
 * All configuration, threshold, and data registers are reset
 * to their power-on state through a power-on reset.
 * The reset bit is cleared back to zero after reset finishes.
 */
void MAX3010X::softReset(void) {
	bitMask(REG_MODECONFIG, MASK_RESET, RESET);

	// Poll for bit to clear, reset is then complete
	// Timeout after 100ms
	uint32_t startTime = to_ms_since_boot(get_absolute_time());
	while (to_ms_since_boot(get_absolute_time()) - startTime < 100)
	{
		uint8_t response = readRegister(_i2caddr, REG_MODECONFIG);
		// uint8_t response = i2c_smbus_read_byte_data(_i2c, REG_MODECONFIG);
		if ((response & RESET) == 0) break; // Done reset!
		busy_wait_ms(1); // Prevent over burden the I2C bus
	}
}

/**
 * Sets which LEDs are used for sampling.
 * - Red only
 * - Red+IR only
 * - Custom
 */
void MAX3010X::setLEDMode(uint8_t mode) {
	bitMask(REG_MODECONFIG, MASK_LEDMODE, mode);
}

/**
 * Sets ADC Range.
 * Available ADC Range: 2048, 4096, 8192, 16384
 */
void MAX3010X::setADCRange(uint8_t adcRange) {
	bitMask(REG_PARTICLECONFIG, MASK_ADCRANGE, adcRange);
}

/**
 * Sets Sample Rate.
 * Available Sample Rates: 50, 100, 200, 400, 800, 1000, 1600, 3200
 */
void MAX3010X::setSampleRate(uint8_t sampleRate) {
	bitMask(REG_PARTICLECONFIG, MASK_SAMPLERATE, sampleRate);
}

/**
 * Sets Pulse Width.
 * Available Pulse Width: 69, 188, 215, 411
 */
void MAX3010X::setPulseWidth(uint8_t pulseWidth) {
	bitMask(REG_PARTICLECONFIG, MASK_PULSEWIDTH, pulseWidth);
}

/**
 * Sets Red LED Pulse Amplitude.
 */
void MAX3010X::setPulseAmplitudeRed(uint8_t amplitude) {
	writeRegister(_i2caddr, REG_LED1_PULSEAMP, amplitude);
	// i2c_smbus_write_byte_data(_i2c, REG_LED1_PULSEAMP, amplitude);
}

/**
 * Sets IR LED Pulse Amplitude.
 */
void MAX3010X::setPulseAmplitudeIR(uint8_t amplitude) {
	writeRegister(_i2caddr, REG_LED2_PULSEAMP, amplitude);
	// i2c_smbus_write_byte_data(_i2c, REG_LED2_PULSEAMP, amplitude);
}

void MAX3010X::setPulseAmplitudeGreen(uint8_t amplitude) {
	writeRegister(_i2caddr, REG_LED3_PULSEAMP, amplitude);
	// i2c_smbus_write_byte_data(_i2c, REG_LED3_PULSEAMP, amplitude);
}

void MAX3010X::setPulseAmplitudeProximity(uint8_t amplitude) {
	writeRegister(_i2caddr, REG_LED_PROX_AMP, amplitude);
	// i2c_smbus_write_byte_data(_i2c, REG_LED_PROX_AMP, amplitude);
}

/**
 * Set the IR ADC count that will trigger the beginning of particle-sensing mode.
 * The threshMSB signifies only the 8 most significant-bits of the ADC count.
 */
void MAX3010X::setProximityThreshold(uint8_t threshMSB) {
	writeRegister(_i2caddr, REG_PROXINTTHRESH, threshMSB);
	// i2c_smbus_write_byte_data(_i2c, REG_PROXINTTHRESH, threshMSB);
}

/**
 * Given a slot number assign a thing to it.
 * Devices are SLOT_RED_LED or SLOT_RED_PILOT (proximity)
 * Assigning a SLOT_RED_LED will pulse LED
 * Assigning a SLOT_RED_PILOT will ??
 */
void MAX3010X::enableSlot(uint8_t slotNumber, uint8_t device) {
	uint8_t originalContents;
	switch (slotNumber) {
		case (1):
			bitMask(REG_MULTILEDCONFIG1, MASK_SLOT1, device);
			break;
		case (2):
			bitMask(REG_MULTILEDCONFIG1, MASK_SLOT2, device << 4);
			break;
		case (3):
			bitMask(REG_MULTILEDCONFIG2, MASK_SLOT3, device);
			break;
		case (4):
			bitMask(REG_MULTILEDCONFIG2, MASK_SLOT4, device << 4);
			break;
		default:
			// Shouldn't be here!
			break;
	}
}

/**
 * Clears all slot assignments.
 */
void MAX3010X::disableSlots(void) {
	writeRegister(_i2caddr, REG_MULTILEDCONFIG1, 0);
	writeRegister(_i2caddr, REG_MULTILEDCONFIG2, 0);
	// i2c_smbus_write_byte_data(_i2c, REG_MULTILEDCONFIG1, 0);
	// i2c_smbus_write_byte_data(_i2c, REG_MULTILEDCONFIG2, 0);
}


// FIFO Configuration //

/**
 * Sets sample average.
 */
void MAX3010X::setFIFOAverage(uint8_t numberOfSamples) {
	bitMask(REG_FIFOCONFIG, MASK_SAMPLEAVG, numberOfSamples);
}

/**
 * Resets all points to start in a known state.
 * Recommended to clear FIFO before beginning a read.
 */
void MAX3010X::clearFIFO(void) {
	writeRegister(_i2caddr, REG_FIFOWRITEPTR, 0);
	writeRegister(_i2caddr, REG_FIFOOVERFLOW, 0);
	writeRegister(_i2caddr, REG_FIFOREADPTR, 0);
	// i2c_smbus_write_byte_data(_i2c, REG_FIFOWRITEPTR, 0);
	// i2c_smbus_write_byte_data(_i2c, REG_FIFOOVERFLOW, 0);
	// i2c_smbus_write_byte_data(_i2c, REG_FIFOREADPTR, 0);
}

/**
 * Enable roll over if FIFO over flows.
 */
void MAX3010X::enableFIFORollover(void) {
	bitMask(REG_FIFOCONFIG, MASK_ROLLOVER, ROLLOVER_ENABLE);
}

/**
 * Disable roll over if FIFO over flows.
 */
void MAX3010X::disableFIFORollover(void) {
        bitMask(REG_FIFOCONFIG, MASK_ROLLOVER, ROLLOVER_DISABLE);
}

/**
 * Sets number of samples to trigger the almost full interrupt.
 * Power on deafult is 32 samples.
 */
void MAX3010X::setFIFOAlmostFull(uint8_t numberOfSamples) {
	bitMask(REG_FIFOCONFIG, MASK_A_FULL, numberOfSamples);
}

/**
 * Read the FIFO Write Pointer.
 */
uint8_t MAX3010X::getWritePointer(void) {
	return (readRegister(_i2caddr, REG_FIFOWRITEPTR));
	// return (i2c_smbus_read_byte_data(_i2c, REG_FIFOWRITEPTR));
}

/**
 * Read the FIFO Read Pointer.
 */
uint8_t MAX3010X::getReadPointer(void) {
	return (readRegister(_i2caddr, REG_FIFOREADPTR));
	// return (i2c_smbus_read_byte_data(_i2c, REG_FIFOREADPTR));
}

/**
 * Die Temperature.
 * Returns temperature in C.
 */
float MAX3010X::readTemperature() {
	// DIE_TEMP_RDY interrupt must be enabled.
	
	// Step 1: Config die temperature register to take 1 temperature sample.
	writeRegister(_i2caddr, REG_DIETEMPCONFIG, 0x01);
	// i2c_smbus_write_byte_data(_i2c, REG_DIETEMPCONFIG, 0x01);

	// Poll for bit to clear, reading is then complete.
	// Timeout after 100ms.
	uint32_t startTime = to_ms_since_boot(get_absolute_time());
	while (to_ms_since_boot(get_absolute_time()) - startTime < 100)
	{
		uint8_t response = readRegister(_i2caddr, REG_INTSTAT2);
		// uint8_t response = i2c_smbus_read_byte_data(_i2c, REG_INTSTAT2);
		if ((response & INT_DIE_TEMP_RDY_ENABLE) > 0) break;
		busy_wait_ms(1);
	}
	
	// Step 2: Read die temperature register (integer)
	int8_t tempInt = readRegister(_i2caddr, REG_DIETEMPINT);
	uint8_t tempFrac = readRegister(_i2caddr, REG_DIETEMPFRAC);
	// int8_t tempInt = i2c_smbus_read_byte_data(_i2c, REG_DIETEMPINT);
	// uint8_t tempFrac = i2c_smbus_read_byte_data(_i2c, REG_DIETEMPFRAC); // causes clearing of the DIE_TEMP_RDY interrupt

	// Step 3: Calculate temperature.
	return (float)tempInt + ((float)tempFrac * 0.0625);
}

/**
 * Returns die temperature in F.
 */
float MAX3010X::readTemperatureF() {
	float temp = readTemperature();

	if (temp != -999.0) temp = temp * 1.8 + 32.0;

	return (temp);
}

/**
 * Sets the PROX_INT_THRESHold.
 */
void MAX3010X::setPROXINTTHRESH(uint8_t val) {
	writeRegister(_i2caddr, REG_PROXINTTHRESH, val);
	// i2c_smbus_write_byte_data(_i2c, REG_PROXINTTHRESH, val);
}


// Device ID and Revision //

uint8_t MAX3010X::readPartID() {
	return readRegister(_i2caddr, REG_PARTID);
	// return i2c_smbus_read_byte_data(_i2c, REG_PARTID);
}

void MAX3010X::readRevisionID() {
	revisionID = readRegister(_i2caddr, REG_REVISIONID);
	// revisionID = i2c_smbus_read_byte_data(_i2c, REG_REVISIONID);
}

uint8_t MAX3010X::getRevisionID() {
	return revisionID;
}


// Setup the Sensor
void MAX3010X::setup(uint8_t powerLevel, uint8_t sampleAverage, uint8_t ledMode, int sampleRate, int pulseWidth, int adcRange) {
	// Reset all configuration, threshold, and data registers to POR values
	softReset();

	// FIFO Configuration //
	
	// The chip will average multiple samples of same type together if you wish
	if (sampleAverage == 1) setFIFOAverage(SAMPLEAVG_1);
	else if (sampleAverage == 2) setFIFOAverage(SAMPLEAVG_2);
	else if (sampleAverage == 4) setFIFOAverage(SAMPLEAVG_4);
	else if (sampleAverage == 8) setFIFOAverage(SAMPLEAVG_8);
	else if (sampleAverage == 16) setFIFOAverage(SAMPLEAVG_16);
	else if (sampleAverage == 32) setFIFOAverage(SAMPLEAVG_32);
	else setFIFOAverage(SAMPLEAVG_4);

	// Allow FIFO to wrap/roll over
	enableFIFORollover();

	// Mode Configuration //
	if (ledMode == 3) setLEDMode(LEDMODE_MULTILED); // Watch all three led channels [TODO] there are only 2!
	else if (ledMode == 2) setLEDMode(LEDMODE_REDIRONLY);
	else setLEDMode(LEDMODE_REDONLY);
	activeLEDs = ledMode; // used to control how many bytes to read from FIFO buffer
	
	// Particle Sensing Configuration //
	if (adcRange < 4096) setADCRange(ADCRANGE_2048);
	else if (adcRange < 8192) setADCRange(ADCRANGE_4096);
	else if (adcRange < 16384) setADCRange(ADCRANGE_8192);
	else if (adcRange == 16384) setADCRange(ADCRANGE_16384);
	else setADCRange(ADCRANGE_2048);
	
	if (sampleRate < 100) setSampleRate(SAMPLERATE_50);
	else if (sampleRate < 200) setSampleRate(SAMPLERATE_100);
	else if (sampleRate < 400) setSampleRate(SAMPLERATE_200);
	else if (sampleRate < 800) setSampleRate(SAMPLERATE_400);
	else if (sampleRate < 1000) setSampleRate(SAMPLERATE_800);
	else if (sampleRate < 1600) setSampleRate(SAMPLERATE_1000);
	else if (sampleRate < 3200) setSampleRate(SAMPLERATE_1600);
	else if (sampleRate == 3200) setSampleRate(SAMPLERATE_3200);
	else setSampleRate(SAMPLERATE_50);

	if (pulseWidth < 118) setPulseWidth(PULSEWIDTH_69);	  // 15 bit resolution
	else if (pulseWidth < 215) setPulseWidth(PULSEWIDTH_118); // 16 bit resolution
	else if (pulseWidth < 411) setPulseWidth(PULSEWIDTH_215); // 17 bit resolution
	else if (pulseWidth == 411) setPulseWidth(PULSEWIDTH_411);// 18 bit resolution
	else setPulseWidth(PULSEWIDTH_69);

	// LED Pulse Amplitude Configuration //
	setPulseAmplitudeRed(powerLevel);
	setPulseAmplitudeIR(powerLevel);
  	setPulseAmplitudeGreen(powerLevel);
	setPulseAmplitudeProximity(powerLevel);

	// Multi-LED Mode Configuration //
	// Enable the readings of the three LEDs [TODO] only 2!
	enableSlot(1, SLOT_RED_LED);
	if (ledMode > 1) enableSlot(2, SLOT_IR_LED);
	if (ledMode > 2) enableSlot(3, SLOT_GREEN_LED);

	// Reset the FIFO before we begin checking the sensor.
	clearFIFO();
}


// Data Collection //

/**
 * Returns the number of samples available.
 */
uint8_t MAX3010X::available(void)
{
	int8_t numberOfSamples = sense.head - sense.tail;
	if (numberOfSamples < 0) numberOfSamples += STORAGE_SIZE;
	return (numberOfSamples);
}

/**
 * Report the most recent Red value.
 */
uint32_t MAX3010X::getRed(void)
{
	if(safeCheck(250))
		return (sense.red[sense.head]);
	else
		return(0);
}

/**
 * Report the most recent IR value.
 */
uint32_t MAX3010X::getIR(void)
{
	if(safeCheck(250))
		return (sense.IR[sense.head]);
	else
		return(0);
}

/**
 * Report the most recent Green value.
*/
uint32_t MAX3010X::getGreen(void)
{
    if(safeCheck(250))
        return (sense.green[sense.head]);
    else
        return(0);
}

/**
 * Report the next Red value in FIFO.
 */
uint32_t MAX3010X::getFIFORed(void)
{
	return (sense.red[sense.tail]);
}

/**
 * Report the next IR value in FIFO.
 */
uint32_t MAX3010X::getFIFOIR(void)
{
	return (sense.IR[sense.tail]);
}

/**
 * Report the next Green value in FIFO
 */
uint32_t MAX3010X::getFIFOGreen(void)
{
	return (sense.green[sense.tail]);
}

/**
 * Advance the tail.
 */
void MAX3010X::nextSample(void)
{
	if(available()) {
		sense.tail++;
		sense.tail %= STORAGE_SIZE;
	}
}

uint16_t MAX3010X::check(void) {
	uint8_t readPointer = getReadPointer();
	uint8_t writePointer = getWritePointer();

	int numberOfSamples = 0;

	// Do we have new data?
	if (readPointer != writePointer) {
		// Calculate the number of readings we need to get from sensor
		numberOfSamples = writePointer - readPointer;
		//std::cout << "There are " << (int)numberOfSamples << " available samples." << std::endl;
		if (numberOfSamples < 0) numberOfSamples += 32;

		// We know have the number of readings, now calculate bytes to read.
		// For this example we are just doing Red and IR (3 bytes each)
		int bytesLeftToRead = numberOfSamples * activeLEDs * 3;

		// //Get ready to read a burst of data from the FIFO register
		i2c_write_blocking(_i2c, _i2caddr, &REG_FIFODATA, 1, true);
		// _i2cPort->beginTransmission(MAX30105_ADDRESS);
		// _i2cPort->write(MAX30105_FIFODATA);
		// _i2cPort->endTransmission();

		// We may need to read as many as 288 bytes so we read in blocks no larger than I2C_BUFFER_LENGTH
		//I2C_BUFFER_LENGTH changes based on the platform. 64 bytes for SAMD21, 32 bytes for Uno.
		//Wire.requestFrom() is limited to BUFFER_LENGTH which is 32 on the Uno
		while (bytesLeftToRead > 0) {
			int toGet = bytesLeftToRead;
			if (toGet > I2C_BUFFER_LENGTH) {
				//If toGet is 32 this is bad because we read 6 bytes (Red+IR * 3 = 6) at a time
				//32 % 6 = 2 left over. We don't want to request 32 bytes, we want to request 30.
				//32 % 9 (Red+IR+GREEN) = 5 left over. We want to request 27.

				// Trim toGet to be a multiple of the samples we need to read.
				toGet = I2C_BUFFER_LENGTH - (I2C_BUFFER_LENGTH % (activeLEDs * 3));
			}

			bytesLeftToRead -= toGet;

			// Request toGet number of bytes from sensor
			int index = 0;
			// i2c_write_blocking(_i2c, _i2caddr, &REG_FIFODATA, 1, true);
			i2c_read_blocking(_i2c, _i2caddr, readMany, toGet, false);

			while (toGet > 0) {
				sense.head++; // Advance the head of the storage struct
				sense.head %= STORAGE_SIZE;

				uint8_t temp[sizeof(uint32_t)]; // Array of 4 bytes that we will convert into long
				uint32_t tempLong;

				// Burst read three bytes - RED
				temp[3] = 0;
				temp[2] = readMany[index++];
				temp[1] = readMany[index++];
				temp[0] = readMany[index++];

				// Convert array to long
				std::memcpy(&tempLong, temp, sizeof(tempLong));
				
				// Zero out all but 18 bits
				tempLong &= 0x3FFFF;
				
				// Store this reading into the sense array
				sense.red[sense.head] = tempLong;

				if (activeLEDs > 1) 
				{
					// Burst read three more bytes - IR
					temp[3] = 0;
					temp[2] = readMany[index++];
					temp[1] = readMany[index++];
					temp[0] = readMany[index++];

					// Convert array to long
					std::memcpy(&tempLong, temp, sizeof(tempLong));
					
					// Zero out all 18 bits
					tempLong &= 0x3FFFF;

					sense.IR[sense.head] = tempLong;
				}

				if (activeLEDs > 2) 
				{
					// Burst read three more bytes - IR
					temp[3] = 0;
					temp[2] = readMany[index++];
					temp[1] = readMany[index++];
					temp[0] = readMany[index++];

					// Convert array to long
					std::memcpy(&tempLong, temp, sizeof(tempLong));
					
					// Zero out all 18 bits
					tempLong &= 0x3FFFF;

					sense.green[sense.head] = tempLong;
				}
				

				toGet -= activeLEDs * 3; 
			}
		}
	}
	return (numberOfSamples);
}

/**
 * Check for new data but give up after a certain amount of time.
 * Returns true if new data was found.
 * Returns false if new data was not found.
 */
bool MAX3010X::safeCheck(uint8_t maxTimeToCheck) {
	uint32_t markTime = to_ms_since_boot(get_absolute_time());

	while(1) {
		uint32_t endTime = to_ms_since_boot(get_absolute_time());
		if (endTime - markTime > maxTimeToCheck) {
			return false;
		}

		if (check() == true) {
			// We found new data!
			return true;
		}

		busy_wait_ms(1);
	}
}

/**
 * Set certain thing in register.
 */
void MAX3010X::bitMask(uint8_t reg, uint8_t mask, uint8_t thing) {
	// Read register
	uint8_t originalContents = readRegister(_i2caddr, reg);
	// uint8_t originalContents = i2c_smbus_read_byte_data(_i2c, reg);

	// Zero-out portions of the register based on mask
	originalContents = originalContents & mask;

	// Change contents of register
	writeRegister(_i2caddr, reg, originalContents | thing);
	// i2c_smbus_write_byte_data(_i2c, reg, originalContents | thing);
}

uint8_t MAX3010X::readRegister(uint8_t address, uint8_t reg) {
	uint8_t res;
	i2c_write_blocking(_i2c, address, &reg, 1, true);
	i2c_read_blocking(_i2c, address, &res, 1, false);

	return res;
}

void MAX3010X::writeRegister(uint8_t address, uint8_t reg, uint8_t val) {
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = val;
	i2c_write_blocking(_i2c, address, buf, 2, true);

	// i2c_write_blocking(_i2c, address, &val, 1, true);
}
