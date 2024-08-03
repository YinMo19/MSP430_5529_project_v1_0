#ifndef __oled_control_h__
#define __oled_control_h__

#include <msp430.h>
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
            return;                                                            \
        }                                                                      \
    }                                                                          \
    while (0)

#define __oled_show__(__to_show, __to_show_2)                                  \
    do {                                                                       \
        OLED_Clear();                                                          \
        OLED_ShowCHinese(0, 0, 0);                                             \
        OLED_ShowCHinese(18, 0, 1);                                            \
        OLED_ShowCHinese(36, 0, 2);                                            \
        OLED_ShowCHinese(54, 0, 3);                                            \
        OLED_ShowCHinese(72, 0, 4);                                            \
        OLED_ShowCHinese(90, 0, 5);                                            \
        OLED_ShowCHinese(108, 0, 6);                                           \
        OLED_ShowString(0, 2, "0.96' OLED TEST");                              \
        OLED_ShowString(20, 4, "2024/08/03");                                  \
        OLED_ShowString(0, 6, __to_show);                                      \
        OLED_ShowString(63, 6, __to_show_2);                                   \
    } while (0)

unsigned int THRESHOLDS[7] = {
    TEMP_THRESHOLD_1,
    TEMP_THRESHOLD_2,
    TEMP_THRESHOLD_3,
    TEMP_THRESHOLD_4,
    TEMP_THRESHOLD_5,
    TEMP_THRESHOLD_6,
    INFINITY,
};

unsigned char *temps[7] = {
    "TEMP1",
    "TEMP2",
    "TEMP3",
    "TEMP4",
    "TEMP5",
    "TEMP6",
    "TEMP7",
};

void oled_control(volatile unsigned int ivalue);

#endif /* __oled_control_h__ */
