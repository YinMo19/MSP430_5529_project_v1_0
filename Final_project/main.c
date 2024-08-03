#include <msp430.h>
#include "Paper_Display.h"
#include "temp_led.h"
#include <stdbool.h>
#include "oled.h"
#include "bmp.h"

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer

    /**
	 * @brief 初始化OLED
	 *
	 */
    u8 t;
    OLED_Init();        //初始化OLED
    OLED_Clear();
    OLED_ShowString(30, 0, "OLED TEST");
    OLED_ShowString(8, 2, "ZHONGJINGYUAN");
    OLED_ShowString(20, 4, "2014/05/01");
    OLED_ShowString(0, 6, "ASCII:");
    OLED_ShowString(63, 6, "CODE:");
    t = ' ';
    OLED_ShowCHinese(0, 0, 0);          //中
    OLED_ShowCHinese(18, 0, 1);         //景
    OLED_ShowCHinese(36, 0, 2);         //园
    OLED_ShowCHinese(54, 0, 3);         //电
    OLED_ShowCHinese(72, 0, 4);         //子
    OLED_ShowCHinese(90, 0, 5);         //科
    OLED_ShowCHinese(108, 0, 6);        //技
    /**/
    t = ' ';

    /**
	 * @brief 初始化 AD
	 *
	 */
    IO_Init();
    ADInit();
    OLED_Init();

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
    // DIS_IMG(1);
    DIS_IMG(2);
    // display(" Huatsing Instruments   ", 42, 16, TimesNewRoman, size8, 1, 1);
    //	display("012abcABC",80, 0,TimesNewRoman,size16,0,0);
    volatile unsigned int ivalue = 0;        //设置判断变量
    while (true) {
        ivalue = Filter();        //软件滤波
		oled_control(ivalue);
        temperature_control(ivalue);
        large_led_breath(ivalue);
    }
}
