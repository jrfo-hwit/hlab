"""
    Class for TCS34725 and TCS34727

    Static instance parameters:
    - I2C address of sensor, optional, default 0x29
    - I2C SCL Pin, mandatory
    - I2C SDA Pin, mandatory
    - I2C frequency, optional, default 400 KHz
    Creating an instance includes activation of I2C communications with sensor.

    Methods:
    - close() - deactivate sensor, terminate I2C communications

    Configurable parameters:
    - autogain (True/False)
    - gain (code 0..3)
    - integration time (code 255..0)

    Read only variables:
    - colors            (tuple of 4 counts: Clear, Red, Green, Blue)
    - device_type       (name of connected sensor)
    - gain_factor       (1/4/16/60)
    - integration_time  (ms)
    - isconnected       (True/False)
    - overflow_count    (value depends on integration time)

    Note: Code based on TCS34725, but TCS34727 is fully equivalent

"""

from time import sleep_ms
from machine import Pin, SoftI2C    # I/O
from micropython import const
from struct import unpack

TCS34725_I2C_ADDR = const(0x29)     # I2C default address
TCS34725_FREQ     = const(400000)   # I2C default baudrate

TCS34725_ID       = const(0x44)     # device ID TCS34725
TCS34727_ID       = const(0x4D)     # device ID TCS34727

TCS3472x_dict    = {TCS34725_ID : "TCS34725",
                    TCS34727_ID : "TCS34727"}

TCSREG_ENABLE    = const(0x00)      # power, ADC enable
TCSREG_ATIME     = const(0x01)      # RGBC integration time
TCSREG_CONFIG    = const(0x0D)      # configuration
TCSREG_CONTROL   = const(0x0F)      # gain control
TCSREG_ID        = const(0x12)      # device ID
TCSREG_STATUS    = const(0x13)      # status
TCSREG_ALLDATA   = const(0x14)      # first of all 4 counts
TCSREG_CDATA     = const(0x14)      # Clear count (as word)
TCSREG_RDATA     = const(0x16)      # Red count
TCSREG_GDATA     = const(0x18)      # Green count
TCSREG_BDATA     = const(0x1A)      # Blue count

TCSCMD_ADDRESS   = const(0xA0)      # command (auto incr. addressing)

TCSCMD_POWER_OFF = const(0x00)
TCSCMD_POWER_ON  = const(0x01)      # Power ON
TCSCMD_AEN       = const(0x02)      # RGBC enable

TCSSTAT_AVALID   = const(0x01)      # AVALID bit in status register

# ADC gain
TCSGAIN_MIN      = const(0)
TCSGAIN_LOW      = const(1)
TCSGAIN_HIGH     = const(2)
TCSGAIN_MAX      = const(3)
TCSGAIN_FACTOR   = (const(1), const(4), const(16), const(60))

# codes for some selected integration times (count-down in steps of 2.4 ms)
# longer integration time gives a larger range of possible counts
TCSINTEG_MIN     = const(255)       # 2.4 ms   max count 1023
TCSINTEG_LOW     = const(252)       # 9.6                4095
TCSINTEG_MEDIUM  = const(240)       # 38.4              16383
TCSINTEG_HIGH    = const(192)       # 153.6             65535
TCSINTEG_MAX     = const(0)         # 614.4             65535


class TCS34725(object):
    """ TCS34725 class
        Create an I2C instance for the PyBoard/ESP32/ESP8266.
        <addr> is I2C address, optional, default 0x29
        <scl> and <sda> are required parameters, to be specified as Pin objects
        <freq> is I2C clock frequency, optional, default 400 KHz
        Default values for gain, integration time and autogain are set,
        but these may be changed any time by the user program.
    """
    def __init__(self, i2c, addr=TCS34725_I2C_ADDR, freq=TCS34725_FREQ):
        self.__addr = addr                                  # I2C address
        self.__buf1 = bytearray(1)                          # one-byte buffer
        self.__buf8 = bytearray(8)                          # eight-byte buffer
        self.__gain = 0                                     # gain code
        self.__integ = 0                                    # integration code
        self.__id = 0x00                                    # device id
        self.__autogain = False                             # no automatic gain
        self.__connected = False                            # no device connected yet      
        self.__bus = i2c                                    # software driver
        try:
            self.__bus.writeto(self.__addr, b'\x80')        # basic write
        except OSError:
            print("Failed to contact device with I2C address 0x{:02x}".format(self.__addr))
            return
        self.__write_register(TCSREG_ENABLE, TCSCMD_POWER_ON | TCSCMD_AEN)  # activate RGBC
        sleep_ms(5)                                         # minimum 2.4ms
        self.__id = self.__read_register(TCSREG_ID)
        if not self.__id in TCS3472x_dict.keys():             # check for supported device
            print("Failed to detect supported color sensor")
            print("Expected ID {:#X} ({:s}) or {:#X} ({:s}), received {:#02X} : {:s}".format(
                   TCS34725_ID, TCS3472x_dict[TCS34725_ID],
                   TCS34727_ID, TCS3472x_dict[TCS34727_ID],
                   self.__id, self.device_type))
            return
        self.__connected = True
        self.gain = TCSGAIN_LOW                             # set initial gain
        self.integ = TCSINTEG_MEDIUM                        # set initial integration

    def __read_register(self, reg):
        """ read register <reg>, return integer value """
        try:
            self.__bus.readfrom_mem_into(self.__addr, TCSCMD_ADDRESS | reg, self.__buf1)
            return self.__buf1[0]
        except Exception as err:
            print("I2C read_register error:", err)
            return -1                                       # nothing received

    def __write_register(self, reg, data):
        """ write register """
        self.__buf1[0] = data
        try:
            self.__bus.writeto_mem(self.__addr, TCSCMD_ADDRESS | reg, self.__buf1)
        except Exception as err:
            print("I2C write_byte error:", err)
            return False
        return True

    def __read_alldata(self):
        """ read all counts (8 contiguous data registers) into local buffer """
        try:
            self.__bus.readfrom_mem_into(self.__addr, TCSCMD_ADDRESS | TCSREG_ALLDATA, self.__buf8)
        except Exception as err:
            print("I2C read_alldata error:", err)
                                                   # nothing received
    def __adjustgain_one_step(self, counts):
        """ adjust gain (if possible!) when certain count limits are reached:
            <counts> is tuple of 4 integers
            switch to lower gain when highest count exceeds 85% of maximum
            switch to higher gain when highest count is below 15% of maximum
            note: quotient of high over low boundaries must be greater
                  than 4 to prevent flip-flopping between gain factors
            return True when gain changed, False otherwise
        """
        UNDERFLOW_PERCENT = const(15)                       # upper count limit (%)
        OVERFLOW_PERCENT = const(85)                        # lower count limit (%)
        # print("overflow_count=", self.overflow_count)
        count_max = max(counts)
        if count_max >= self.overflow_count * OVERFLOW_PERCENT // 100:   # nearing overflow
            if self.gain > TCSGAIN_MIN:                     # currently not in lowest gain mode
                # print("==> Actual gain factor", self.gain_factor, end="")
                self.gain -= 1                              # decrease gain one step
                # print(", new", self.gain_factor)
                return True                                 # switched gain
        elif count_max < self.overflow_count * UNDERFLOW_PERCENT // 100:  # nearing underflow
            if self.gain < TCSGAIN_MAX:                     # currently not in highest gain mode
                # print("==> Actual gain factor:", self.gain_factor, end="")
                self.gain += 1                              # increase gain one step
                # print(", new", self.gain_factor)
                return True                                 # switch gain
        return False                                        # no gain change


    """ Public methods and properties """

    def close(self):
        """ Power-down device and close I2C bus (if supported) """
        self.__write_register(TCSREG_ENABLE, TCSCMD_POWER_OFF)  # deactivate sensor
        if "deinit" in dir(self.__bus):
            self.__bus.deinit()                             # de-activate I2C
        self.__connected = False

    # Reminder: a property is referenced like a variable (not as a method)

    @property
    def isconnected(self):
        """ return status of the connection """
        return self.__connected                             # True/False

    @property
    def device_type(self):
        """ return name (string) of connected sensor """
        return TCS3472x_dict.get(self.__id, "Unknown")

    @property
    def gain(self):
        """ return current gain code """
        return self.__gain

    @gain.setter
    def gain(self, gain):
        """ set gain code (0..3), forced to a value within limits """
        self.__gain = max(TCSGAIN_MIN, min(TCSGAIN_MAX, gain))  # new gain code
        self.__write_register(TCSREG_CONTROL, self.__gain)      # update gain
        sleep_ms(2 * self.integration_time)                    # longer than integ time

    @property
    def gain_factor(self):
        """ return current gain factor """
        return TCSGAIN_FACTOR[self.__gain]

    @property
    def autogain(self):
        """ return current autogain setting """
        return self.__autogain

    @autogain.setter
    def autogain(self, autogain_new):
        """ change autogain setting (True / False) """
        self.__autogain = True if autogain_new == True else False

    @property
    def integ(self):
        """ return current integrationtime code code """
        return self.__integ

    @integ.setter
    def integ(self, integ):
        """ set integrationtime code (255..0), forced to a value within limits """
        self.__integ = max(TCSINTEG_MAX, min(TCSINTEG_MIN, integ))  # new integ code
        self.__write_register(TCSREG_ATIME, self.__integ)           # update integration time
        sleep_ms(2 * self.integration_time)                        # longer than integ time

    @property
    def integration_time(self):
        """ return current integration time in milliseconds """
        return int(2.4 * (256 - self.__integ))

    @property
    def overflow_count(self):
        """ return maximum count for actual integration time """
        return min(65535, (256 - self.__integ) * 1024)

    @property
    def colors(self):
        """ read all data registers, return tuple: (clear, red, green, blue) """
        self.__read_alldata()
        format_counts = "<HHHH"                                 # 4 USHORTs (16-bits, Little Endian)
        counts = unpack(format_counts, self.__buf8);
        if self.__autogain == True:
            while self.__adjustgain_one_step(counts):           # adjust gain
                self.__read_alldata()                           # re-read with new settings
                counts = unpack(format_counts, self.__buf8)
        return counts

#
