from machine import I2C, Timer, Pin, SoftI2C
import time
import gc

class bq25622:

    def from_bytes(self, bytes, byteorder='big', signed=False):
        if byteorder == 'little':
            little_ordered = list(bytes)
        elif byteorder == 'big':
            little_ordered = list(reversed(bytes))
        else:
            raise ValueError("byteorder must be either 'little' or 'big'")

        n = sum(b << i*8 for i, b in enumerate(little_ordered))
        if signed and little_ordered and (little_ordered[-1] & 0x80):
            n -= 1 << 8*len(little_ordered)
        return n

    # endereço do carregador de bateria
    BatteryCharger_ADDR=0x6b
    # registradores do carregador de bateria
    REG0X02_Charge_Current_Limit = 0x02
    REG0X04_Charge_Voltage_Limit = 0x04
    REG0X06_Input_Current_Limit = 0x06
    REG0X08_Input_Voltage_Limit = 0x08
    REG0X0A_IOTG_regulation = 0x0A
    REG0X0C_VOTG_regulation = 0x0C
    REG0X0E_Minimal_System_Voltage = 0x0E
    REG0X10_Pre_charge_Control = 0x10
    REG0X12_Termination_Control = 0x12
    REG0X14_Charge_Control_0 = 0x14
    REG0X15_Charge_Timer_Control = 0x15
    REG0X16_Charger_Control_1 = 0x16
    REG0X17_Charger_Control_2 = 0x17
    REG0X18_Charger_Control_3 = 0x18
    REG0X19_Charger_Control_4 = 0x19
    REG0X1A_NTC_Control_0 = 0x1A
    REG0X1B_NTC_Control_1 = 0x1B
    REG0X1C_NTC_Control_2 = 0x1C
    REG0X1D_Charger_Status_0 = 0x1D
    REG0X1E_Charger_Status_1 = 0x1E
    REG0X1F_FAULT_Status_0 = 0x1F
    REG0X20_Charger_Flag_0 = 0x20
    REG0X21_Charger_Flag_1 = 0x21
    REG0X22_FAULT_Flag_0 = 0x22
    REG0X23_Charger_Mask_0 = 0x23
    REG0X24_Charger_Mask_1 = 0x24
    REG0X25_FAULT_Mask_0 = 0x25
    REG0X26_ADC_Control = 0x26
    REG0X27_ADC_Function_Disable_0 = 0x27
    REG0X28_IBUS_ADC = 0x28
    REG0X2A_IBAT_ADC = 0x2A
    REG0X2C_VBUS_ADC = 0x2C
    REG0X2E_VPMID_ADC = 0x2E
    REG0X30_VBAT_ADC = 0x30
    REG0X32_VSYS_ADC = 0x32
    REG0X34_TS_ADC = 0x34
    REG0X36_TDIE_ADC = 0x36
    REG0X38_Part_Information = 0x38

    def __init__(self, i2c, address=BatteryCharger_ADDR):
        # Free up hardware resources
        gc.collect()
        
        self._i2c = i2c
        self._address = address
        #------------configurações do BQ25622------------
        # Ignorar NTC (porque o mesmo não está conectado)
        self._i2c.writeto_mem(self._address, self.REG0X1A_NTC_Control_0, b'\xBD')
        # Escrever corrente de carga limite. write charge current limit 1040mA 0x0D<<6 = 0x0340
        self._i2c.writeto_mem(self._address, self.REG0X02_Charge_Current_Limit, b'\x40')
        self._i2c.writeto_mem(self._address, self.REG0X02_Charge_Current_Limit+1, b'\x03')
        # desativar WDT. Devido termos dado comandos no i2c, o chip entra em modo host ativando o watchdog, então precisamos desativar o watchdog para não ter que ficar dando comando toda hora para resetar o mesmo.
        #self._i2c.writeto_mem(self._address, self.REG0X16_Charger_Control_1, b'\xA0')

        # write charge voltage limit 4400mV 0x01B8<<3 = 0x0DC0
        #self._i2c.writeto_mem(self._address, self.REG0X04_Charge_Voltage_Limit, b'\xC0')
        #self._i2c.writeto_mem(self._address, self.REG0X04_Charge_Voltage_Limit+1, b'\x0D')
        
        # resetar WDT. precisamos fazer em um intervalo menor que 50 segundos.
        self._i2c.writeto_mem(self._address, self.REG0X16_Charger_Control_1, b'\xA5')

    def wdt(self):
        # resetar WDT. precisamos fazer em um intervalo menor que 50 segundos.
        self._i2c.writeto_mem(self._address, self.REG0X16_Charger_Control_1, b'\xA5')

    def adc_single_conversion(self):
        self.wdt()
        # ADC control register 0xC0 = ADC_EN , ADC_RATE (single shot), ADC_sample (12 effective bits), ADC_AVG (single value), ADC_AVG_INIT (start average using existing value)
        self._i2c.writeto_mem(self._address, self.REG0X26_ADC_Control, b'\xc0')

        # wait for ADC_EN be cleared (conversion finished)
        val = self._i2c.readfrom_mem(self._address, self.REG0X26_ADC_Control, 1)
        while val[0] >= 128:
            time.sleep(0.1)
            val = self._i2c.readfrom_mem(self._address, self.REG0X26_ADC_Control, 1)

    def ibus_adc(self):
        self.wdt()
        # read IBUS
        val = self._i2c.readfrom_mem(self._address, self.REG0X28_IBUS_ADC, 2)
        data = self.from_bytes(val, byteorder='little', signed=True)
        data = (data >> 1) * 2#mA
        return data
        #str_ibus="ibus: {:5.0f} mA".format(data)
        #print(str_ibus)

    def ibat_adc(self):
        self.wdt()
        # read IBAT
        val = self._i2c.readfrom_mem(self._address, self.REG0X2A_IBAT_ADC, 2)
        data = self.from_bytes(val, byteorder='little', signed=True)
        data = (data >> 2) * 4#mA
        return data
        #str_ibat="ibat: {:5.0f} mA".format(data)
        #print(str_ibat)

    def vbus_adc(self):
        self.wdt()
        # read VBUS
        val = self._i2c.readfrom_mem(self._address, self.REG0X2C_VBUS_ADC, 2)
        vbus = (((val[0] + (val[1]<<8))>>2) * 3.97)/1000#V
        return vbus
        #str_vbus="vbus: {:5.2f} V".format(vbus)
        #print(str_vbus)

    def vpmid_adc(self):
        self.wdt()
        # read Vpmid
        val = self._i2c.readfrom_mem(self._address, self.REG0X2E_VPMID_ADC, 2)
        vpmid = (((val[0] + (val[1]<<8))) * 3.97)/1000#V, isso tá estranho pq tá dando 20v
        return vpmid
        #print("vpimd: {:5.2f} V".format(vpmid))

    def vbat_adc(self):
        self.wdt()
        # read VBAT
        val = self._i2c.readfrom_mem(self._address, self.REG0X30_VBAT_ADC, 2)
        vbat = (((val[0] + (val[1]<<8))>>1) * 1.99)/1000#V
        return vbat
        #str_vbat="vbat: {:5.2f} V".format(vbat)
        #print(str_vbat)

    def vsys_adc(self):
        self.wdt()
        # read Vsys
        val = self._i2c.readfrom_mem(self._address, self.REG0X32_VSYS_ADC, 2)
        vsys = (((val[0] + (val[1]<<8))>>1) * 1.99)/1000#V
        return vsys
        #str_vsys="vsys: {:5.2f} V".format(vsys)
        #print(str_vsys)

    def ts_adc(self):
        self.wdt()
        val = self._i2c.readfrom_mem(self._address, self.REG0X34_TS_ADC, 2)
        ts = (((val[0] + (val[1]<<8))) * 0.0961)#V
        return ts
        #print("ts: {:5.2f} %".format(ts))

    
    def adc_die_temperature(self):
        self.wdt()
        val = self._i2c.readfrom_mem(self._address, self.REG0X36_TDIE_ADC, 2)
        data = self.from_bytes(val, byteorder='little', signed=True)
        data = (data) * 0.5#degC
        return data
        #str_tdie="tdie: {:5.2f} C".format(data)
        #print(str_tdie)
    
    def dev_rev(self):
        self.wdt()
        val = self._i2c.readfrom_mem(self._address, self.REG0X38_Part_Information, 1)
        revision = val[0] & 0x7  
        device = ((val[0] & 0x38) >> 3) # isso tá estranho pois tá apresentando 0x03 e só era pra ser 0 ou 1
        return revision

    def bat_debug(self):
        self.wdt()
        # reader charge current limit
        val = self._i2c.readfrom_mem(self._address, self.REG0X02_Charge_Current_Limit, 2)
        vbus = (((val[0] + (val[1]<<8))>>6) * 80) #mA
        str_vbus="ichL: {:5.2f} mA".format(vbus)
        print(str_vbus)
        
        # read charge voltage limit
        val = self._i2c.readfrom_mem(self._address, self.REG0X04_Charge_Voltage_Limit, 2)
        vbus = (((val[0] + (val[1]<<8))>>3) * 10)/1000 #mV
        str_vbus="vchL: {:5.2f} V".format(vbus)
        print(str_vbus)

        # read input current limit register
        val = self._i2c.readfrom_mem(self._address, self.REG0X06_Input_Current_Limit, 2)
        vbus = (((val[0] + (val[1]<<8))>>4) * 20) #mA
        str_vbus="iinL: {:5.2f} mA".format(vbus)
        print(str_vbus)

        # read input voltage limit register
        val = self._i2c.readfrom_mem(self._address, self.REG0X08_Input_Voltage_Limit, 2)
        vbus = (((val[0] + (val[1]<<8))>>5) * 40)/1000 #mV
        str_vbus="vinL: {:5.2f} V".format(vbus)
        print(str_vbus)

        # read IOTG regulation register
        val = self._i2c.readfrom_mem(self._address, self.REG0X0A_IOTG_regulation, 2)
        vbus = (((val[0] + (val[1]<<8))>>4) * 20) #mA
        str_vbus="iotgR: {:5.2f} mA".format(vbus)
        print(str_vbus)

        # read VOTG regulation register
        val = self._i2c.readfrom_mem(self._address, self.REG0X0C_VOTG_regulation, 2)
        vbus = (((val[0] + (val[1]<<8))>>6) * 80)/1000 #mV
        str_vbus="votgR: {:5.2f} V".format(vbus)
        print(str_vbus)

        # read minimal system voltage register
        val = self._i2c.readfrom_mem(self._address, self.REG0X0E_Minimal_System_Voltage, 2)
        vbus = (((val[0] + (val[1]<<8))>>6) * 80)/1000 #mV
        str_vbus="vmsR: {:5.2f} V".format(vbus)
        print(str_vbus)

        # read pre-charge control register
        val = self._i2c.readfrom_mem(self._address, self.REG0X10_Pre_charge_Control, 2)
        vbus = (((val[0] + (val[1]<<8))>>4) * 20) #mA
        str_vbus="iotgR: {:5.2f} mA".format(vbus)
        print(str_vbus)

        # read termination control register
        val = self._i2c.readfrom_mem(self._address, self.REG0X12_Termination_Control, 2)
        vbus = (((val[0] + (val[1]<<8))>>3) * 10) #mA
        str_vbus="itctl: {:5.2f} mA".format(vbus)
        print(str_vbus)

        # read charge control register 0
        val = self._i2c.readfrom_mem(self._address, self.REG0X14_Charge_Control_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ch_Ctl0: " + hex(val)
        print(str_vbus)

        # read charge control register
        val = self._i2c.readfrom_mem(self._address, self.REG0X15_Charge_Timer_Control, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chT_Ctl: " + hex(val)
        print(str_vbus)

        # read charge control register 1
        val = self._i2c.readfrom_mem(self._address, self.REG0X16_Charger_Control_1, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ch_Ctl1: " + hex(val)
        print(str_vbus)

        # read charge control register 2
        val = self._i2c.readfrom_mem(self._address, self.REG0X17_Charger_Control_2, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ch_Ctl2: " + hex(val)
        print(str_vbus)

        # read charge control register 3
        val = self._i2c.readfrom_mem(self._address, self.REG0X18_Charger_Control_3, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ch_Ctl3: " + hex(val)
        print(str_vbus)

        # read charge control register 4
        val = self._i2c.readfrom_mem(self._address, self.REG0X19_Charger_Control_4, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ch_Ctl4: " + hex(val)
        print(str_vbus)
        
        # read NTC control register 0
        val = self._i2c.readfrom_mem(self._address, self.REG0X1A_NTC_Control_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ntctl0: " + hex(val)
        print(str_vbus)

        # read NTC control register 1
        val = self._i2c.readfrom_mem(self._address, self.REG0X1B_NTC_Control_1, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ntctl1: " + hex(val)
        print(str_vbus)

        # read NTC control register 2
        val = self._i2c.readfrom_mem(self._address, self.REG0X1C_NTC_Control_2, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="ntctl2: " + hex(val)
        print(str_vbus)
        
        # read REG0X1D_Charger_Status_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X1D_Charger_Status_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_st0: " + hex(val)
        print(str_vbus)

        # read REG0X1E_Charger_Status_1
        val = self._i2c.readfrom_mem(self._address, self.REG0X1E_Charger_Status_1, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_st1: " + hex(val)
        print(str_vbus)

        # read REG0X1F_FAULT_Status_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X1F_FAULT_Status_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_fault_st0: " + hex(val)
        print(str_vbus)

        # read REG0X20_Charger_Flag_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X20_Charger_Flag_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_flag0: " + hex(val)
        print(str_vbus)

        # read REG0X21_Charger_Flag_1
        val = self._i2c.readfrom_mem(self._address, self.REG0X21_Charger_Flag_1, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_flag1: " + hex(val)
        print(str_vbus)

        # read REG0X22_FAULT_Flag_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X22_FAULT_Flag_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_fault_flag0: " + hex(val)
        print(str_vbus)
        
        # read REG0X23_Charger_Mask_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X23_Charger_Mask_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_mask0: " + hex(val)
        print(str_vbus)

        # read REG0X24_Charger_Mask_1
        val = self._i2c.readfrom_mem(self._address, self.REG0X24_Charger_Mask_1, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_mask1: " + hex(val)
        print(str_vbus)

        # read REG0X25_FAULT_Mask_0
        val = self._i2c.readfrom_mem(self._address, self.REG0X25_FAULT_Mask_0, 1)
        val = self.from_bytes(val, byteorder='little', signed=False)
        str_vbus="chg_fault_mask0: " + hex(val)
        print(str_vbus)
        
        print('-------------------')

