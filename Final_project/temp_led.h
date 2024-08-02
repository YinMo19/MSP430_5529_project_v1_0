#ifndef __temp_led_h__
#define __temp_led_h__

#define N_POINT 13

extern unsigned int icnt;

void                ADInit();
unsigned int        GetAD();
unsigned int        Filter();
unsigned int        GetAD1();
unsigned int        Filter1();
void                IO_Init(void);
void                temperature_control(volatile unsigned int ivalue);

#endif        //__temp_led_h__
