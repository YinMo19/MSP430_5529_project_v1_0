#ifndef __temp_led_h__
#define __temp_led_h__

#define N_POINT          13
#define TEMP_THRESHOLD_1 2550
#define TEMP_THRESHOLD_2 2600
#define TEMP_THRESHOLD_3 2650
#define TEMP_THRESHOLD_4 2700
#define TEMP_THRESHOLD_5 2750
#define TEMP_THRESHOLD_6 2800

#define DELAY_MAX        1600
#define DELAY_MEDIUM     800
#define DELAY_MIN        16
#define BREATHING_PAUSE  1600000
#define MAX_BREATH_VALUE 8000

#define breathe(delay, pause)                                                  \
    do {                                                                       \
        int i = 0;                                                             \
        for (i = 0; i < MAX_BREATH_VALUE; i++) {                               \
            TA2CCR2 = i;                                                       \
            __delay_cycles(delay);                                             \
        }                                                                      \
        __delay_cycles(pause);                                                 \
        for (i = MAX_BREATH_VALUE; i > 0; i--) {                               \
            TA2CCR2 = i;                                                       \
            __delay_cycles(delay);                                             \
        }                                                                      \
        __delay_cycles(pause);                                                 \
    } while (0)

#define set_output_if_above_threshold(threshold, port, bit, ivalue)            \
    {                                                                          \
        if (ivalue >= threshold) {                                             \
            port |= bit;                                                       \
        } else {                                                               \
            port &= ~bit;                                                      \
        }                                                                      \
    }                                                                          \
    while (0)

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
