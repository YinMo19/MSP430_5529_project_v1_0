#include <msp430.h>
#include "Paper_Display.h"
#include "temp_led.h"

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer
    IO_Init();

    ADInit();

    P8DIR |= BIT1;
    P8OUT &= ~BIT1;

    initClock();
    PaperIO_Int();
    INIT_SSD1673();
    Init_buff();
    DIS_IMG(2);

    volatile unsigned int ivalue = 0;        //设置判断变量
    while (1) {
        //  ivalue = GetAD();                                            //没有软件滤波
        ivalue = Filter();        //软件滤波
        temperature_control(ivalue);
    }
}
