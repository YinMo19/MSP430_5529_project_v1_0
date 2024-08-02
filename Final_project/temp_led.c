#include <msp430.h>
#include "temp_led.h"

void ADInit() {
    ADC12CTL0  |= ADC12MSC;             //自动循环采样转换
    ADC12CTL0  |= ADC12ON;              //启动ADC12模块
    ADC12CTL1  |= ADC12CONSEQ_3;        //选择序列通道多次循环采样转换
    ADC12CTL1  |= ADC12SHP;             //采样保持模式
    ADC12CTL1  |= ADC12CSTARTADD_0;
    ADC12MCTL0 |= ADC12INCH_1;        //通道选择
    ADC12MCTL1 |= ADC12INCH_2 + ADC12EOS;
    ADC12CTL0  |= ADC12ENC;
}

unsigned int GetAD() {
    unsigned int temp  = 0;                //设置变量
    ADC12CTL0         |= ADC12SC;          //开始采样转换
    temp               = ADC12MEM0;        //把结果赋给变量
    return temp;
}

unsigned int Filter() {
    char         count, i, j;
    unsigned int value_buf[N_POINT];
    unsigned int temp = 0;
    int          sum  = 0;
    for (count = 0; count < N_POINT; count++) {
        value_buf[count] = GetAD();
    }
    for (j = 0; j < N_POINT - 1; j++) {
        for (i = 0; i < N_POINT - j - 1; i++) {
            if (value_buf[i] > value_buf[i + 1]) {
                temp             = value_buf[i];
                value_buf[i]     = value_buf[i + 1];
                value_buf[i + 1] = temp;
            }
        }
    }
    for (count = 1; count < N_POINT - 1; count++)
        sum += value_buf[count];
    //如果为2的n次方等分，则可以用>>n的减少计算量  a=a*8;  可以为 a=a<<3;
    //b=b/8; 可以为b=b>>3;
    return (unsigned int)(sum / (N_POINT - 2));
}

unsigned int GetAD1() {
    unsigned int temp  = 0;                //设置变量
    ADC12CTL0         |= ADC12SC;          //开始采样转换
    temp               = ADC12MEM1;        //把结果赋给变量
    return temp;
}

unsigned int Filter1() {
    char         count, i, j;
    unsigned int value_buf[N_POINT];
    unsigned int temp = 0;
    int          sum  = 0;
    for (count = 0; count < N_POINT; count++) {
        value_buf[count] = GetAD1();
    }
    for (j = 0; j < N_POINT - 1; j++) {
        for (i = 0; i < N_POINT - j - 1; i++) {
            if (value_buf[i] > value_buf[i + 1]) {
                temp             = value_buf[i];
                value_buf[i]     = value_buf[i + 1];
                value_buf[i + 1] = temp;
            }
        }
    }
    for (count = 1; count < N_POINT - 1; count++)
        sum += value_buf[count];
    //如果为2的n次方等分，则可以用>>n的减少计算量  a=a*8;  可以为 a=a<<3;
    //b=b/8; 可以为b=b>>3;
    return (unsigned int)(sum / (N_POINT - 2));
}

void IO_Init(void) {
    P8DIR |= BIT1;         // 设置 P8.1 引脚为输出模式
    P8OUT &= ~BIT1;        // 确保 P8.1 初始状态为低电平

    P3DIR |= BIT7;         // 设置 P3.7 引脚为输出模式
    P3OUT &= ~BIT7;        // 确保 P3.7 初始状态为低电平

    P6DIR |= BIT3;         // 设置 P6.3 引脚为输出模式
    P6OUT &= ~BIT3;        // 确保 P6.3 初始状态为低电平
    P6DIR |= BIT4;         // 设置 P6.4 引脚为输出模式
    P6OUT &= ~BIT4;        // 确保 P6.4 初始状态为低电平

    P7DIR |= BIT4;         // 设置 P7.4 引脚为输出模式
    P7OUT &= ~BIT4;        // 确保 P7.4 初始状态为低电平
    P3DIR |= BIT5;         // 设置 P3.5 引脚为输出模式
    P3OUT &= ~BIT5;        // 确保 P3.5 初始状态为低电平
}

void temperature_control(volatile unsigned int ivalue) {
    if (ivalue >= 2550) {
        P3OUT |= BIT5;
    } else {
        P3OUT &= ~BIT5;
    }
    if (ivalue >= 2600) {
        P6OUT |= BIT4;
    } else {
        P6OUT &= ~BIT4;
    }
    if (ivalue >= 2650) {
        P6OUT |= BIT3;
    } else {
        P6OUT &= ~BIT3;
    }
    if (ivalue >= 2700) {
        P7OUT |= BIT4;
    } else {
        P7OUT &= ~BIT4;
    }
    if (ivalue >= 2750) {
        P3OUT |= BIT7;
    } else {
        P3OUT &= ~BIT7;
    }
    if (ivalue >= 2800) {
        P8OUT |= BIT1;
    } else {
        P8OUT &= ~BIT1;
    }
}
