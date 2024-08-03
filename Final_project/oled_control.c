#include "oled_control.h"
#include "temp_led.h"

void oled_control(volatile unsigned int ivalue) {
    static unsigned int last_flag = 7;
    int                 i         = 0;
    for (i = 0; i < 7; i++) {
        set_oled_if_above_threshold(
            THRESHOLDS[i], temps[i], "CODE", ivalue, last_flag, i);
    }
}
