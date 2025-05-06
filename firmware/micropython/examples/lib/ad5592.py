from machine import Pin, SPI
import time
import gc

class AD5592R:
    # Control register command
    # MSB 15 is 0 => control register address	& Zeros			& control register data
    # 		15 (1b)		14-11 (4b)					10-9 (2b)		8-0 (9b)
    
    # DAC set command
    # MSB 15 is 1 => DAC address	& 12-bit DAC data
    # 		15 (1b)		14-11 (4b)		11-0 (12b)
    
    # Adapted from: https://github.com/Metaln00b/Arduino-libad5592r/blob/master/libad5592r.cpp
    
    # Register addresses
    AD5592R_REG_NOP = 0x00			# No Operation
    AD5592R_REG_DAC_RD = 0x01		#
    AD5592R_REG_ADC_SEQ = 0x02
    AD5592R_REG_GEN_CTRL_REG = 0x03
    AD5592R_REG_ADC_CONFIG = 0x04
    AD5592R_REG_DAC_CONFIG = 0x05
    AD5592R_REG_PULLDWN_CONFIG = 0x06
    AD5592R_REG_CONFIG_READ_AND_LDAC = 0x07
    AD5592R_REG_GPIO_CONFIG = 0x08
    AD5592R_REG_GPIO_OUTPUT = 0x09
    AD5592R_REG_GPIO_INPUT = 0x0A
    AD5592R_REG_PD_REF_CTRL = 0x0B
    AD5592R_REG_GPIO_OPENDRAIN_CONFIG = 0x0C
    AD5592R_REG_IO_TS_CONFIG = 0x0D
    # does not exist 0x0E register
    AD5592R_REG_SW_RESET = 0x0F


    AD5592R_CNTRL_ADDRESS_MASK = 0x7800  # Control register bit mask */

    AD5592R_CMD_NOP = 0x0000  # No operation */
    AD5592R_CMD_DAC_READBACK = 0x0800  # Selects and enables DAC read back */
    AD5592R_CMD_ADC_READ = 0x1000  # Selects ADCs for conversion */
    AD5592R_CMD_GP_CNTRL = 0x1800  # General purpose control register */
    AD5592R_CMD_ADC_PIN_SELECT = 0x2000  # Selects which pins are ADC inputs */
    AD5592R_CMD_DAC_PIN_SELECT = 0x2800  # Selects which pins are DAC outputs */
    AD5592R_CMD_PULL_DOWN_SET = 0x3000  # Selects which pins have 85kOhm pull-down resistor to GND */
    AD5592R_CMD_CNTRL_REG_READBACK = 0x3800  # Read back control registers and/or set LDAC */
    AD5592R_CMD_GPIO_WRITE_CONFIG = 0x4000  # Selects which pins are GPIO outputs */
    AD5592R_CMD_GPIO_WRITE_DATA = 0x4800  # Writes data to the GPIO outputs */
    AD5592R_CMD_GPIO_READ_CONFIG = 0x5000  # Selects which pins are GPIO inputs */
    AD5592R_CMD_GPIO_READ_INPUT = 0x5400  # Read GPIO inputs */
    AD5592R_CMD_POWER_DWN_REF_CNTRL = 0x5800  # Powers down DACs and enables/disables the reference */
    AD5592R_CMD_GPIO_DRAIN_CONFIG = 0x6000  # Selects open-drain or push/pull for GPIO outputs */
    AD5592R_CMD_THREE_STATE_CONFIG = 0x6800  # Selects which pins are three-state */
    AD5592R_CMD_SW_RESET = 0x7DAC  # Software reset of the AD5592R */

    AD5592R_PIN_SELECT_MASK = 0x00FF  # Pin select bit mask */
    # DAC Register Definitionen */
    AD5592R_DAC_WRITE_MASK = 0x8000  # DAC write bit mask */
    AD5592R_DAC_ADDRESS_MASK = 0x7000  # DAC pin address bit mask */
    AD5592R_DAC_VALUE_MASK = 0x0FFF  # DAC output value bit mask */

    # Range Selection 2xVref */
    AD5592R_ADC_TT_VREF = 0x0010  # Set ADC input range to 2 times Vref */
    AD5592R_DAC_TT_VREF = 0x0020  # Set DAC output range to 2 times Vref */

    def __init__(self, spi, cs, rst=0xff):
        # Free up hardware resources
        gc.collect()
        
        self.spi = spi
        self.cs = cs
        self.cs(1)
        if rst != 0xff:
            self.rst = rst
            self.rst(0)
            self.rst(1)
        # reset
        self._wr_register(self.AD5592R_CMD_SW_RESET)
        self.gpio_states = 0x00  # Keep track of GPIO output states
        
    def _wr_register(self, reg_value):
        txdata = reg_value.to_bytes(2)
        rxdata = bytearray(2)
        self.cs(0)
        self.spi.write_readinto(txdata, rxdata)
        self.cs(1)
        return int.from_bytes(rxdata)

    def init_chip(self, dac_channels=[], adc_channels=[], gpio_in_channels=[], gpio_out_channels=[], internal_reference=1):
        # Convert lists to bitmasks
        dac_mask = 0 + sum([1 << ch for ch in dac_channels])
        print("dac_mask {:}".format(hex(dac_mask)))
        adc_mask = 0 + sum([1 << ch for ch in adc_channels])
        print("adc_mask {:}".format(hex(adc_mask)))
        gpio_in_mask = 0 + sum([1 << ch for ch in gpio_in_channels])
        print("gpio_in_mask {:}".format(hex(gpio_in_mask)))
        gpio_out_mask = 0 + sum([1 << ch for ch in gpio_out_channels])
        print("gpio_out_mask {:}".format(hex(gpio_out_mask)))
        
        # Enable internal reference 2v5
        self._wr_register(self.AD5592R_CMD_POWER_DWN_REF_CNTRL | (0x2 << 8))
        print("internal reference")

        # general control
        self._wr_register(self.AD5592R_CMD_GP_CNTRL) # vref single
        # AD5592R_CMD_GP_CNTRL | AD5592R_ADC_TT_VREF | AD5592R_DAC_TT_VREF # Vref double

        # ADC and DAC
        self._wr_register(self.AD5592R_CMD_ADC_PIN_SELECT | adc_mask) # adc pins
        self._wr_register(self.AD5592R_CMD_DAC_PIN_SELECT | dac_mask) # dac pins

        # TBD - Configure GPIO in and out
        self._wr_register(self.AD5592R_CMD_GPIO_WRITE_CONFIG | gpio_out_mask)
        #self._wr_register(self.AD5592R_CMD_POWER_DWN_REF_CNTRL | (~gpio_in_mask & 0xFF))
        #self._wr_register(self.AD5592R_CMD_POWER_DWN_REF_CNTRL | (gpio_in_mask & 0xFF))
        #self._wr_register(self.AD5592R_CMD_THREE_STATE_CONFIG | (~gpio_in_mask & 0xFF))
        #self._wr_register(self.AD5592R_CMD_THREE_STATE_CONFIG | (gpio_in_mask & 0xFF))
        #self._wr_register(self.AD5592R_CMD_GPIO_DRAIN_CONFIG | (~gpio_in_mask & 0xFF))
        #self._wr_register(self.AD5592R_CMD_GPIO_DRAIN_CONFIG | (gpio_in_mask & 0xFF))
        self._wr_register(self.AD5592R_CMD_GPIO_READ_CONFIG | gpio_in_mask)
    
    def set_dac(self, channel, value):
        data = self._wr_register(self.AD5592R_DAC_WRITE_MASK | ((channel << 12) & self.AD5592R_DAC_ADDRESS_MASK) | value)

    def read_adc(self, channel):
        assert 0 <= channel <= 7
        invalid = self._wr_register(self.AD5592R_CMD_ADC_READ | (0x1 << channel))
        invalid = self._wr_register(self.AD5592R_CMD_NOP) #nop command
        data = self._wr_register(self.AD5592R_CMD_NOP) #nop command
        return (data & 0x0FFF)

    def gpio_get(self, channel):
        assert 0 <= channel <= 7
        invalid = self._wr_register(self.AD5592R_CMD_GPIO_READ_INPUT | (0x1 << channel))
        data = self._wr_register(self.AD5592R_CMD_NOP)
        #data = self._wr_register(self.AD5592R_CMD_NOP)
        return (data >> channel)   

    def gpio_set(self, channel, value):
        assert 0 <= channel <= 7  
        if value == 0:
            self.gpio_states = self.gpio_states & ((~(1 << channel)) & 0xFF)
        else:
            self.gpio_states = self.gpio_states | (1 << channel)
        invalid = self._wr_register(self.AD5592R_CMD_GPIO_WRITE_DATA | self.gpio_states)
    
    def print_all_registers(self):
        """ Print all of the registers in a neat table.
            Skips over CONFIG_READ_AND_LDAC since it's the register used to read each register and is modified during the read!
        """
        reg_names = ["NOP", "DAC_RD", "ADC_SEQ", "GEN_CTRL_REG", "ADC_CONFIG",
                     "DAC_CONFIG", "PULLDWN_CONFIG", "CONFIG_READ_AND_LDAC",
                     "GPIO_W_CONFIG", "GPIO_W_OUTPUT", "GPIO_R_IN_CFG", "PD_REF_CTRL",
                     "GPIO_OPENDRAIN_CONFIG", "IO_TS_CONFIG", "RESERVED1", "SW_RST"]
        longest_len = len(max(reg_names, key = len))
        for reg_id in range(0x10):
            if reg_names[reg_id] == 'RESERVED1' or reg_names[reg_id] == 'RESERVED2' or reg_names[reg_id] == 'RESERVED3':
                continue
            print(f"{reg_names[reg_id]:<{longest_len}} {reg_id:#06b} {self._wr_register(self.AD5592R_CMD_CNTRL_REG_READBACK | (1<<6) | (reg_id<<2)):#018b}")
            
