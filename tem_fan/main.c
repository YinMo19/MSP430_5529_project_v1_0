#include <msp430.h>

void main(void)
{
    // 停止看门狗计时器
    WDTCTL = WDTPW + WDTHOLD;

    // 设置P2.4为输出
    P2DIR |= BIT4;
    P2OUT |= BIT4; // 设置P2.4为高电平

    while (1)
    {
        // 在此处可以添加其他代码，或者保持空闲
    }
}
