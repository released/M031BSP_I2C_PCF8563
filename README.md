# M031BSP_I2C_PCF8563
 M031BSP_I2C_PCF8563


update @ 2024/09/09

1. init I2C : PC1 : SCL , PC0 : SDA

2. PCF8563 board SCL , SDA , CLKOUT need to pull high

3.  refer to 

    https://www.waveshare.net/wiki/PCF8563_RTC_Board
	
    https://github.com/adafruit/RTClib/blob/master/src/RTClib.cpp
	
    https://github.com/torvalds/linux/blob/master/drivers/rtc/rtc-pcf8563.c

4. with first power on , will trig set date / time as below 

![image](https://github.com/released/M031BSP_I2C_PCF8563/blob/main/log.jpg)


5. when PCF8563 board already powered and initialed , below is log message when MCU perform chip reset ( RTC time will remain)

![image](https://github.com/released/M031BSP_I2C_PCF8563/blob/main/log_reset_chip.jpg)


6. CLKOUT freq set to 1024Hz , below is waveform capture 

![image](https://github.com/released/M031BSP_I2C_PCF8563/blob/main/waveform_CLKOUT.jpg)


