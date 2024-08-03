#ifndef __oled_control_h__
#define __oled_control_h__

#include "temp_led.h"
#include "oled.h"
#define INFINITY 65534

#define set_oled_if_above_threshold(                                           \
    threshold, __to_show, __to_show_2, ivalue, last_flag, flag)                \
    {                                                                          \
        if (ivalue <= threshold) {                                             \
            if (flag != last_flag) {                                           \
                __oled_show__(__to_show, __to_show_2);                         \
                last_flag = flag;                                              \
            }                                                                  \
            __delay_cycles(2000000);                                           \
            return;                                                            \
        }                                                                      \
    }                                                                          \
    while (0)

#define __oled_show__(__to_show, __to_show_2)                                  \
    do {                                                                       \
        OLED_Clear();                                                          \
        OLED_ShowString(0, 0, "TemperatureAlarm");                             \
        OLED_ShowString(0, 2, "By YinMo19 & LZJ");                             \
        OLED_ShowString(20, 4, "2024/08/03");                                  \
        OLED_ShowString(0, 6, __to_show);                                      \
        OLED_ShowString(45, 6, __to_show_2);                                   \
    } while (0)

void oled_control(volatile unsigned int ivalue);

#endif /* __oled_control_h__ */
