#include <msp430.h>
#include "temp_led.h"

// 最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
void ClockInit(void) {             // 最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
    UCSCTL6 &= ~XT1OFF;            // 启动XT1
    P5SEL   |= BIT2 + BIT3;        // XT2引脚功能选择
    UCSCTL6 &= ~XT2OFF;            // 打开XT2
    __bis_SR_register(SCG0);
    UCSCTL0 = DCO0 + DCO1 + DCO2 + DCO3 + DCO4;
    UCSCTL1 = DCORSEL_4;         // DCO频率范围在28.2MHZ以下
    UCSCTL2 = FLLD_5 + 1;        // D=16，N=1

    // n=8,FLLREFCLK时钟源为XT2CLK；
    // DCOCLK=D*(N+1)*(FLLREFCLK/n);
    // DCOCLKDIV=(N+1)*(FLLREFCLK/n);
    UCSCTL3 = SELREF_5 + FLLREFDIV_3;
    // ACLK的时钟源为DCOCLKDIV,
    // MCLK\SMCLK的时钟源为DCOCLK
    UCSCTL4 = SELA_4 + SELS_3 + SELM_3;
    // ACLK由DCOCLKDIV的32分频得到，
    // SMCLK由DCOCLK的2分频得到
    UCSCTL5 = DIVA_5 + DIVS_1;
}

void ADInit() {
    ADC12CTL0 |= ADC12MSC;             //自动循环采样转换
    ADC12CTL0 |= ADC12ON;              //启动ADC12模块
    ADC12CTL1 |= ADC12CONSEQ_3;        //选择序列通道多次循环采样转换
    ADC12CTL1  |= ADC12SHP;            //采样保持模式
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
    unsigned int count, i, j;
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
    unsigned int count, i, j;
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
    P8DIR    |= BIT1;         // 设置 P8.1 引脚为输出模式
    P8OUT    &= ~BIT1;        // 确保 P8.1 初始状态为低电平

    P3DIR    |= BIT7;         // 设置 P3.7 引脚为输出模式
    P3OUT    &= ~BIT7;        // 确保 P3.7 初始状态为低电平

    P6DIR    |= BIT3;         // 设置 P6.3 引脚为输出模式
    P6OUT    &= ~BIT3;        // 确保 P6.3 初始状态为低电平
    P6DIR    |= BIT4;         // 设置 P6.4 引脚为输出模式
    P6OUT    &= ~BIT4;        // 确保 P6.4 初始状态为低电平

    P7DIR    |= BIT4;         // 设置 P7.4 引脚为输出模式
    P7OUT    &= ~BIT4;        // 确保 P7.4 初始状态为低电平
    P3DIR    |= BIT5;         // 设置 P3.5 引脚为输出模式
    P3OUT    &= ~BIT5;        // 确保 P3.5 初始状态为低电平

    P1DIR    |= BIT5;                 // 设置 P1.5 为输出模式
    P1OUT    |= BIT5;                 // 将 P1.5 设置为高电平
    P2DIR    |= (BIT4 + BIT5);        // 设置 P2.4 和 P2.5 为输出模式
    P2OUT    &= ~BIT4;                // 将 P2.4 设置为低电平
    P2SEL    |= BIT5;                 // 选择 P2.5 为大功率LED

    TA0CTL    = TASSEL_2 + MC_1 + TACLR;
    // 时钟源为 SMCLK，增计数模式，清零计数器
    TA0CCR0   = 8000;            // 设置 PWM 周期
    TA0CCTL1  = OUTMOD_7;        // 设置输出模式为重置/置位模式
    TA0CCR1   = 0;               // 初始占空比为 0

    TA2CTL    = TASSEL_2 + TACLR + MC_1;
    TA2CCR0   = 8000;            // PWM周期
    TA2CCTL2  = OUTMOD_7;        // 输出模式7

    P1DIR    |= BIT2;        // 设置 P1.2 为输出方向
    P1SEL    |= BIT2;        // 设置 P1.2 为 Timer_A 输出
}

void temperature_control(volatile unsigned int ivalue) {
    set_output_if_above_threshold(TEMP_THRESHOLD_1, P3OUT, BIT5, ivalue);
    set_output_if_above_threshold(TEMP_THRESHOLD_2, P6OUT, BIT4, ivalue);
    set_output_if_above_threshold(TEMP_THRESHOLD_3, P6OUT, BIT3, ivalue);
    set_output_if_above_threshold(TEMP_THRESHOLD_4, P7OUT, BIT4, ivalue);
    set_output_if_above_threshold(TEMP_THRESHOLD_5, P3OUT, BIT7, ivalue);
    set_output_if_above_threshold(TEMP_THRESHOLD_6, P8OUT, BIT1, ivalue);
}

/**
 * 控制LED灯的呼吸效果
 * 
 * 该函数通过调整TA1CCR2的值来控制LED的亮度，实现呼吸效果
 * 不同的输入值（ivalue）对应不同的延迟周期，从而实现不同的呼吸效果
 * 在温控指示灯不亮时不亮，在温控灯亮起时开始呼吸，
 * 温控灯越多，呼吸越急促，当温控灯亮起4盏，则常亮。
 * 
 * @param ivalue 输入值，用于决定呼吸效果的类型
 */
void led_breath(volatile unsigned int ivalue) {
    if (ivalue >= TEMP_THRESHOLD_1 && ivalue < TEMP_THRESHOLD_2) {
        breathe(DELAY_MAX, BREATHING_PAUSE);
    } else if (ivalue >= TEMP_THRESHOLD_2 && ivalue < TEMP_THRESHOLD_3) {
        breathe(DELAY_MEDIUM, BREATHING_PAUSE);
    } else if (ivalue >= TEMP_THRESHOLD_3 && ivalue < TEMP_THRESHOLD_4) {
        breathe(DELAY_MIN, BREATHING_PAUSE);
    } else if (ivalue >= TEMP_THRESHOLD_4) {
        TA0CCR1 = MAX_BREATH_VALUE;
    } else {
        TA0CCR1 = 0;
    }
}
