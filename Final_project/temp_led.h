#ifndef __temp_led_h__
#define __temp_led_h__

#define N_POINT 13

extern unsigned int icnt;

void                ADInit(void);
unsigned int        GetAD(void);
unsigned int        Filter(void);
unsigned int        GetAD1(void);
unsigned int        Filter1(void);
void                IO_Init(void);
void                temperature_control(volatile unsigned int ivalue);
void                large_led_breath(volatile unsigned int ivalue);

#endif        //__temp_led_h__
