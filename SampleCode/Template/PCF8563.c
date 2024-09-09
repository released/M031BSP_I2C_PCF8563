/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "NuMicro.h"

#include "misc_config.h"

#include "i2c_driver.h"
#include "PCF8563.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

/*
    refer to 
    https://www.waveshare.net/wiki/PCF8563_RTC_Board
    https://github.com/adafruit/RTClib/blob/master/src/RTClib.cpp
    https://github.com/torvalds/linux/blob/master/drivers/rtc/rtc-pcf8563.c

*/

void PCF8563_IIC_Write(unsigned char reg,unsigned char data)
{
                                                                    
    i2c_reg_write(PCF8563_I2CADDR_7BIT,reg, &data, 1);    
}

unsigned char PCF8563_IIC_Read(unsigned char reg)
{    
    unsigned char tmp = 0;

    i2c_reg_read(PCF8563_I2CADDR_7BIT,reg, &tmp , 1);
    return tmp;
}

void PCF8563_Set_Time(int hour, int minute, int second)
{
    if(hour>=0){
        hour = changeIntToHex(hour%24);//60
        PCF8563_IIC_Write(HOURS, hour);
    }
    if(minute>=0){
        minute = changeIntToHex(minute%60);
        PCF8563_IIC_Write(MINUTES, minute);
    }
    if(second>=0){
        second = changeIntToHex(second%60);
        PCF8563_IIC_Write(VL_SECONDS, second);
    }
}

void PCF8563_Set_Days(int year, int months, int days)
{

    if(days > 1 && days <= 31){
        days = changeIntToHex(days);
        PCF8563_IIC_Write(DAYS, days);
    }
    if(months > 1 && months <= 12 ){
        months = changeIntToHex(months);
        PCF8563_IIC_Write(CENTURY_MONTHS, (PCF8563_IIC_Read(CENTURY_MONTHS)&0x80)| months);
    }
    
    if(year>=1900 && year<2000){
        
        PCF8563_IIC_Write(CENTURY_MONTHS, PCF8563_IIC_Read(CENTURY_MONTHS) | 0x80);
        /*7  century; this bit is toggled when the years register
            overflows from 99 to 00
            0 indicates the century is 20xx
            1 indicates the century is 19xx
        */
        year = changeIntToHex(year);
        PCF8563_IIC_Write(YEARS, year%100);

    }else if(year>= 2000 && year < 3000){
        PCF8563_IIC_Write(CENTURY_MONTHS, PCF8563_IIC_Read(CENTURY_MONTHS) & 0x7F);
        
        year = changeIntToHex(year);
        PCF8563_IIC_Write(YEARS, year%100);
    }
}

void PCF8563_Get_Days(unsigned char *buf)
{
    buf[0] = PCF8563_IIC_Read(DAYS)&0x3f;
    buf[1] = PCF8563_IIC_Read(CENTURY_MONTHS) & 0x1f;
    buf[2] = PCF8563_IIC_Read(YEARS)&0xff;
    
    buf[0] = changeHexToInt(buf[0]);
    buf[1] = changeHexToInt(buf[1]);
    buf[2] = changeHexToInt(buf[2]);
	
    if(PCF8563_IIC_Read(CENTURY_MONTHS) & 0x80){
        buf[3] = 19;
    }else{
        buf[3] = 20;
    }
}

void PCF8563_Set_WeekData(int WeekData)
{
    if(WeekData <= 7){
        PCF8563_IIC_Write(WEEKDAYS, WeekData);
    }
}

void PCF8563_Get_Time(unsigned char *buf)
{
    buf[0] = PCF8563_IIC_Read(VL_SECONDS) & 0x7f; //get second data
    buf[1] = PCF8563_IIC_Read(MINUTES) & 0x7f; //get minute data
    buf[2] = PCF8563_IIC_Read(HOURS) & 0x3f; //get hour data

    buf[0] = changeHexToInt(buf[0]);
    buf[1] = changeHexToInt(buf[1]);
    buf[2] = changeHexToInt(buf[2]);
}

void PCF8563_Set_Alarm(int hour, int minute)
{
    if(minute >= 0){
        minute = changeIntToHex(minute);
        PCF8563_IIC_Write(MINUTE_ALARM, minute);
    }
    
    if(hour >= 0){
        hour = changeIntToHex(hour); 
        PCF8563_IIC_Write(HOUR_ALARM, hour);
    }
 
}

void PCF8563_Alarm_Enable(void)
{
    
    PCF8563_IIC_Write(CONTROL_STATUS_2, (PCF8563_IIC_Read(CONTROL_STATUS_2)|0x02));
    PCF8563_IIC_Write(MINUTE_ALARM, PCF8563_IIC_Read(MINUTE_ALARM)&0x7f);
    PCF8563_IIC_Write(HOUR_ALARM, PCF8563_IIC_Read(HOUR_ALARM)&0x7f);
    PCF8563_IIC_Write(DAY_ALARM,0x80); //OFF
    PCF8563_IIC_Write(WEEKDAY_ALARM,0x80);//OFF
}

void PCF8563_Alarm_Disable(void)
{
    PCF8563_IIC_Write(CONTROL_STATUS_2, (PCF8563_IIC_Read(CONTROL_STATUS_2)&0xfd));
    PCF8563_IIC_Write(MINUTE_ALARM, PCF8563_IIC_Read(MINUTE_ALARM)|0x80);
    PCF8563_IIC_Write(HOUR_ALARM, PCF8563_IIC_Read(HOUR_ALARM)|0x80);
    PCF8563_IIC_Write(DAY_ALARM,0x80); //OFF
    PCF8563_IIC_Write(WEEKDAY_ALARM,0x80);//OFF
}



/******************************************************************************
function: Set timer register
parameter:
    Timer_Frequency : Choose the corresponding frequency
                    4096    :TIMER_FREQUENCY_4096
                    64      :TIMER_FREQUENCY_64
                    1       :TIMER_FREQUENCY_1
                    0       :TIMER_FREQUENCY_1_60
    Value           : Value
                    Total cycle = Value/TIMER_FREQUENCY
Info:
    TIMER_CONTROL//0x0E
    TIMER_FREQUENCY_4096    0 // 4096HZ      MAX  0.062 second
    TIMER_FREQUENCY_64      1 // 64HZ        MAX  3.98 second
    TIMER_FREQUENCY_1       2 // 1HZ         MAX  255 second
    TIMER_FREQUENCY_1_60    3 // 1/60Hz      MAX  255 minute
******************************************************************************/
void PCF8563_Set_Timer(int Timer_Frequency ,unsigned char Value)
{
    // PCF8563_IIC_Write(TIMER_CONTROL, PCF8563_IIC_Read(TIMER_CONTROL)&0x7f);
    PCF8563_IIC_Write(TIMER_COUNT, Value);
    if(Timer_Frequency == 4096){
        PCF8563_IIC_Write(TIMER_CONTROL, ((PCF8563_IIC_Read(TIMER_CONTROL))&0xfc)|TIMER_FREQUENCY_4096);
    }else if(Timer_Frequency == 64){
        PCF8563_IIC_Write(TIMER_CONTROL, ((PCF8563_IIC_Read(TIMER_CONTROL))&0xfc)|TIMER_FREQUENCY_64);
    }else if(Timer_Frequency == 1){
        PCF8563_IIC_Write(TIMER_CONTROL, ((PCF8563_IIC_Read(TIMER_CONTROL))&0xfc)|TIMER_FREQUENCY_1);
    }else if(Timer_Frequency == 0){// 1/60
        PCF8563_IIC_Write(TIMER_CONTROL, ((PCF8563_IIC_Read(TIMER_CONTROL))&0xfc)|TIMER_FREQUENCY_1_60);
    }else{
        printf("Set Timer Error\r\n");
    }
}

void PCF8563_Timer_Enable(void)
{
    PCF8563_IIC_Write(CONTROL_STATUS_2, (PCF8563_IIC_Read(CONTROL_STATUS_2)|0x01));
    PCF8563_IIC_Write(TIMER_CONTROL, PCF8563_IIC_Read(TIMER_CONTROL)|0x80);
}

void PCF8563_Timer_Disable(void)
{
    PCF8563_IIC_Write(CONTROL_STATUS_2, (PCF8563_IIC_Read(CONTROL_STATUS_2)&0xfe));
    PCF8563_IIC_Write(TIMER_CONTROL, PCF8563_IIC_Read(TIMER_CONTROL)&0x7f);
}

void PCF8563_Cleare_AF_Flag(void)
{
    PCF8563_IIC_Write(CONTROL_STATUS_2, PCF8563_IIC_Read(CONTROL_STATUS_2)&0xf7);
}

void PCF8563_Cleare_TF_Flag(void)
{
    PCF8563_IIC_Write(CONTROL_STATUS_2, PCF8563_IIC_Read(CONTROL_STATUS_2)&0xfB);
}

/******************************************************************************
function: Get Flag
parameter:
Info:
    return: 1:AF alarm
            2:TF timer
            3:AF and TF
******************************************************************************/
unsigned char PCF8563_Get_Flag(void)
{
    unsigned char temp=0;
    if(PCF8563_IIC_Read(CONTROL_STATUS_2)&0x08){
        temp  =  temp|0x01;
    }
    if(PCF8563_IIC_Read(CONTROL_STATUS_2)&0x04){
        temp  =  temp|0x02;
    }
    return temp;
    
}
/******************************************************************************
function: Set timer register
parameter:
    Timer_Frequency : Choose the corresponding frequency
                    32768   :327.68KHz
                    1024    :1024Hz
                    32      :32Hz
                    1       :1Hz
    Value           : Value
                    Total cycle = Value/TIMER_FREQUENCY
Info:

******************************************************************************/
void PCF8563_CLKOUT_FrequencyHZ(unsigned short Frequency)
{
    if(Frequency == 32768){
        PCF8563_IIC_Write(CLKOUT_CONTROL, (PCF8563_IIC_Read(CLKOUT_CONTROL)&0xfC)|0x00);
    }else if(Frequency == 1024){
        PCF8563_IIC_Write(CLKOUT_CONTROL, (PCF8563_IIC_Read(CLKOUT_CONTROL)&0xfC)|0x01);
    }else if(Frequency == 32){
        PCF8563_IIC_Write(CLKOUT_CONTROL, (PCF8563_IIC_Read(CLKOUT_CONTROL)&0xfC)|0x02);
    }else if(Frequency == 1){
        PCF8563_IIC_Write(CLKOUT_CONTROL, (PCF8563_IIC_Read(CLKOUT_CONTROL)&0xfC)|0x03);
    }else {
        printf("Set CLKOUT requency Selection Error\r\n");
    }
}

void PCF8563_CLKOUT_Enable(void)
{
    PCF8563_IIC_Write(CLKOUT_CONTROL, PCF8563_IIC_Read(CLKOUT_CONTROL)|0x80);
}

void PCF8563_CLKOUT_Disable(void)
{
    PCF8563_IIC_Write(CLKOUT_CONTROL, PCF8563_IIC_Read(CLKOUT_CONTROL)&0x7f);
}


unsigned char conv2d(const char *p) 
{
    unsigned char v = 0;
    if ('0' <= *p && *p <= '9')
        v = *p - '0';
    return 10 * v + *++p - '0';
}

void PCF8563_DateTime(const char *date, const char *time) 
{
    /*
        Sep  9 2024
        18:08:43    
    */

    unsigned char buffer[7] = {0};
    unsigned char yOff; ///< Year offset from 2000
    unsigned char m;    ///< Month 1-12
    unsigned char d;    ///< Day 1-31
    unsigned char hh;   ///< Hours 0-23
    unsigned char mm;   ///< Minutes 0-59
    unsigned char ss;   ///< Seconds 0-59    

    yOff = conv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec
    switch (date[0]) 
    {
        case 'J':
            m = (date[1] == 'a') ? 1 : ((date[2] == 'n') ? 6 : 7);
            break;
        case 'F':
            m = 2;
            break;
        case 'A':
            m = date[2] == 'r' ? 4 : 8;
            break;
        case 'M':
            m = date[2] == 'r' ? 3 : 5;
            break;
        case 'S':
            m = 9;
            break;
        case 'O':
            m = 10;
            break;
        case 'N':
            m = 11;
            break;
        case 'D':
            m = 12;
            break;
    }

    d = conv2d(date + 4);
    hh = conv2d(time);
    mm = conv2d(time + 3);
    ss = conv2d(time + 6);

    buffer[0] = changeIntToHex(ss);
    buffer[1] = changeIntToHex(mm);
    buffer[2] = changeIntToHex(hh);
    buffer[3] = changeIntToHex(d);
    buffer[4] = changeIntToHex(0);
    buffer[5] = changeIntToHex(m);
    buffer[6] = changeIntToHex(yOff);
    // printf("0x%02X\r\n",yOff);

    i2c_reg_write(PCF8563_I2CADDR_7BIT,VL_SECONDS, buffer, SIZEOF(buffer)); 
}

unsigned char PCF8563_lostPower(void)
{
    // return (PCF8563_IIC_Read(VL_SECONDS) >> 7) & 0x01 ;
    return ( PCF8563_IIC_Read(VL_SECONDS) & PCF8563_SC_LV );
}

void PCF8563_start(void)
{
    unsigned char ctlreg = PCF8563_IIC_Read(CONTROL_STATUS_1);
    if (ctlreg & (1 << 5))
    {
        PCF8563_IIC_Write(CONTROL_STATUS_1, ctlreg & ~(1 << 5));
    }
}

void PCF8563_stop(void)
{
    unsigned char ctlreg = PCF8563_IIC_Read(CONTROL_STATUS_1);
    if (!(ctlreg & (1 << 5)))
    {
        PCF8563_IIC_Write(CONTROL_STATUS_1, ctlreg | (1 << 5));
    }
}

void Byte2Bin(unsigned char tmp)
{
    printf("%d ",tmp & 0x80 ? (1) : (0));
    printf("%d ",tmp & 0x40 ? (1) : (0));
    printf("%d ",tmp & 0x20 ? (1) : (0));
    printf("%d ",tmp & 0x10 ? (1) : (0));
    printf("%d ",tmp & 0x08 ? (1) : (0));
    printf("%d ",tmp & 0x04 ? (1) : (0));
    printf("%d ",tmp & 0x02 ? (1) : (0));
    printf("%d ",tmp & 0x01 ? (1) : (0));
}

void PCF8563_get_reg_default(void)
{

    /*
        Address     Register            name Bit
                                        7 6 5 4 3 2 1 0
        00h         Control_status_1    0 0 0 0 1 0 0 0
        01h         Control_status_2    0 0 0 0 0 0 0 0
        02h         VL_seconds          1 x x x x x x x
        03h         Minutes             x x x x x x x x
        04h         Hours               x x x x x x x x
        05h         Days                x x x x x x x x
        06h         Weekdays            x x x x x x x x
        07h         Century_months      x x x x x x x x
        08h         Years               x x x x x x x x
        09h         Minute_alarm        1 x x x x x x x
        0Ah         Hour_alarm          1 x x x x x x x
        0Bh         Day_alarm           1 x x x x x x x
        0Ch         Weekday_alarm       1 x x x x x x x
        0Dh         CLKOUT_control      1 x x x x x 0 0
        0Eh         Timer_control       0 x x x x x 1 1
        0Fh         Timer               x x x x x x x x    
    
    
    */

    unsigned char reg = 0;
    unsigned char tmp = 0;

    for( reg = 0x00 ; reg <= 0xF ; reg++)
    {
        tmp = PCF8563_IIC_Read(reg);
        printf("[0x%02X]:0x%02X--",reg,tmp);

        Byte2Bin(tmp);

        printf("\r\n");
    }

}

void RTC_polling(void)
{
	unsigned char buf[10] = {0};

    PCF8563_Get_Time(buf);
    PCF8563_Get_Days(&buf[3]);
    printf("%d%d/%d/%d , %d:%d:%d\r\n",buf[6],buf[5],buf[4],buf[3],buf[2],buf[1],buf[0]);

    switch(PCF8563_Get_Flag())
    {
        case 1:
            printf("Alarm !!!!!!!!!!!!!\r\n");
            PCF8563_Cleare_AF_Flag();
            break;
        case 2:
            printf("Timer !!!!!!!!!!!!!\r\n");
            PCF8563_Cleare_TF_Flag();
            break;
        case 3:
            printf("Timer and Alarm !!!!!!!!!!!!!\r\n");
            PCF8563_Cleare_TF_Flag();
            PCF8563_Cleare_AF_Flag();
            break;
    }

}

void RTC_Init(void)
{
    // PCF8563_get_reg_default();

    PCF8563_IIC_Write(CONTROL_STATUS_1,0x00);  //basic setting
    PCF8563_IIC_Write(CONTROL_STATUS_2,0x00);  //Disable INT

    PCF8563_Alarm_Disable();
    PCF8563_Timer_Disable();
    PCF8563_CLKOUT_Disable();

    if (PCF8563_lostPower())
    {
        printf("RTC is NOT initialized, set the time\r\n");

        // PCF8563_Set_Time(15,59,58);     //Time
        // PCF8563_Set_Days(2024,9,2);     //Days
        
        PCF8563_DateTime(__DATE__ ,__TIME__ );
    }

    PCF8563_start();
    
    PCF8563_Set_Alarm(0,0);     //Alarm
    PCF8563_Alarm_Disable();

    PCF8563_Set_Timer(1,10);    //Timer 10S
    PCF8563_Timer_Enable();

    PCF8563_CLKOUT_FrequencyHZ(1024);
    PCF8563_CLKOUT_Enable();    
}

