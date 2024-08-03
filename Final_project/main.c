#include <msp430.h>
#include <stdbool.h>
#include "bmp.h"
#include "Paper_Display.h"
#include "temp_led.h"
#include "oled.h"
#include "oled_control.h"

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer

    OLED_Init();        // 初始化OLED

    IO_Init();
    ADInit();
    ClockInit();        // led 初始化

    initClock();        // 初始化时钟，控制电子墨水屏
    PaperIO_Int();
    INIT_SSD1673();
    Init_buff();

    // DIS_IMG(1);
    DIS_IMG(2);
    // display(" Huatsing Instruments   ", 42, 16, TimesNewRoman, size8, 1, 1);
    // display("012abcABC", 80, 0, TimesNewRoman, size16, 0, 0);

    volatile unsigned int ivalue = 0;        //设置判断变量
    while (true) {
        ivalue = Filter();        //软件滤波
        oled_control(ivalue);
        temperature_control(ivalue);
        led_breath(ivalue);
    }
}
