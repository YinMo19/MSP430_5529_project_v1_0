#include <msp430.h>
#include "Paper_Display.h"
#include "temp_led.h"
#include <stdbool.h>

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer

    /**
	 * @brief 初始化 AD 
	 * 
	 */
    IO_Init();
    ADInit();

    // P8DIR |= BIT1;
    // P8OUT &= ~BIT1;

    /**
	 * @brief 初始化时钟，控制电子墨水屏
	 * 
	 */
    initClock();
    PaperIO_Int();
    INIT_SSD1673();
    Init_buff();
    DIS_IMG(2);

    volatile unsigned int ivalue = 0;        //设置判断变量
    while (true) {
        ivalue = Filter();        //软件滤波
        temperature_control(ivalue);
        large_led_breath(ivalue);
    }
}
