#include "oled_control.h"
#include "temp_led.h"

unsigned int __THRESHOLDS[7] = {
    TEMP_THRESHOLD_1,
    TEMP_THRESHOLD_2,
    TEMP_THRESHOLD_3,
    TEMP_THRESHOLD_4,
    TEMP_THRESHOLD_5,
    TEMP_THRESHOLD_6,
    INFINITY,
};

unsigned char *__temps[7] = {
    "TEMP1",
    "TEMP2",
    "TEMP3",
    "TEMP4",
    "TEMP5",
    "TEMP6",
    "TEMP7",
};

unsigned char *__code[7] = {
    "LOWEST_SAFE",
    "LOWER",
    "MEDIUM_ALARM",
    "MEDIUM+_ALARM",
    "HIGH_ALARM",
    "HIGH+_ALARM",
    "ALARM!!!!",
};

void oled_control(volatile unsigned int ivalue) {
    static unsigned int last_flag = 7;
    unsigned int        i         = 0;
    for (i = 0; i < 7; i++) {
        set_oled_if_above_threshold(
            __THRESHOLDS[i], __temps[i], __code[i], ivalue, last_flag, i);
    }
}
