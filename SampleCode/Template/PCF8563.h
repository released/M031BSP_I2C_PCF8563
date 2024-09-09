/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include "NuMicro.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

/*_____ D E F I N I T I O N S ______________________________________________*/

/*
    Read: A3h (10100011)
    Write: A2h (10100010)
*/
#define PCF8563_I2CADDR_8BIT        (0xA2)
#define PCF8563_I2CADDR_7BIT        (PCF8563_I2CADDR_8BIT >> 1)

/*
    Control and status registers
*/
#define CONTROL_STATUS_1            0x00
#define CONTROL_STATUS_2            0x01

/*
    counters for the clock function (seconds up to years counters).
    Time and date registers
*/
#define VL_SECONDS                  0x02
#define MINUTES                     0x03
#define HOURS                       0x04

#define DAYS                        0x05
#define WEEKDAYS                    0x06
#define CENTURY_MONTHS              0x07
#define YEARS                       0x08

/*
    alarm registers which define the conditions for an alarm
    Alarm registers
*/
#define MINUTE_ALARM                0x09
#define HOUR_ALARM                  0x0A
#define DAY_ALARM                   0x0B
#define WEEKDAY_ALARM               0x0C

/*
    CLKOUT output frequency
    CLKOUT control register
*/
#define CLKOUT_CONTROL              0x0D

/*
    Timer_control and Timer registers
    Timer registers
*/
#define TIMER_CONTROL               0x0E
#define TIMER_COUNT                 0x0F

#define PCF8563_SC_LV		        0x80 /* low voltage */

#define changeIntToHex(dec)         ((((dec)/10) <<4) + ((dec)%10) )    // bin2bcd
#define converseIntToHex(dec)       ((((dec)>>4) *10) + ((dec)%16) )    // bcd2bin

#define changeHexToInt(hex)         ((((hex)>>4) *10 ) + ((hex)%16) )
#define converseHexToInt(hex)       ((((hex)/10) <<4 ) + ((hex)%10) )

#define bcd2bin(val)				{val - 6 * (val >> 4)}
#define bin2bcd(val)				{val + 6 * (val / 10)}

#define TIMER_FREQUENCY_4096        0 // 4096HZ
#define TIMER_FREQUENCY_64          1 // 64HZ
#define TIMER_FREQUENCY_1           2 // 1HZ
#define TIMER_FREQUENCY_1_60        3 // 1/60Hz

#define Error_Month(dec)            (dec==4|dec==6|dec==9|dec==11)


/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/


void PCF8563_Init(void);

void PCF8563_Set_Time(int hour, int minute, int second);
void PCF8563_Get_Time(unsigned char *buf);

void PCF8563_Set_Days(int year, int months, int days);
void PCF8563_Get_Days(unsigned char *buf);

void PCF8563_Set_WeekData(int WeekData);

void PCF8563_Set_Alarm(int hour, int minute);
void PCF8563_Set_Timer(int Timer_frequency ,unsigned char value);

void PCF8563_Alarm_Enable(void);
void PCF8563_Alarm_Disable(void);
void PCF8563_Timer_Enable(void);
void PCF8563_Timer_Disable(void);

void PCF8563_Cleare_AF_Flag(void);//Alarm
void PCF8563_Cleare_TF_Flag(void);//Timer

unsigned char  PCF8563_Get_Flag(void);

void PCF8563_CLKOUT_FrequencyHZ(unsigned short Frequency);

void PCF8563_CLKOUT_Disable(void);
void PCF8563_CLKOUT_Enable(void);


void RTC_polling(void);
void RTC_Init(void);

