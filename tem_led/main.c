#include <msp430.h>

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

#define N_POINT 13
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

unsigned int icnt;
void         IO_Init(void) {
    P8DIR |= BIT1;         // 设置P8.1口为输出模式  控制LED灯
    P8OUT &= ~BIT1;        // 选中P8.1为输出方式
    P3DIR |= BIT7;         // 设置P8.1口为输出模式  控制LED灯
    P3OUT &= ~BIT7;        // 选中P8.1为输出方式
                           //
    //    P1DIR |= BIT0;        // 设置P3.6口为输出模式  控制蜂鸣器
    //    P1OUT |= BIT0;        // 选中P3.6为输出方式

    //    P2DIR &= ~(BIT3 + BIT6);
    //    P2REN |= BIT3 + BIT6;
    //    P2OUT |= BIT3 + BIT6;
    //
    //    P1DIR &= ~(BIT3 + BIT2);
    //    P1REN  = BIT3 + BIT2;
    //    P1OUT |= BIT3 + BIT2;

    P6DIR |= BIT3;         // 设置P8.1口为输出模式  控制LED灯
    P6OUT &= ~BIT3;        // 选中P8.1为输出方式
    P6DIR |= BIT4;         // 设置P8.1口为输出模式  控制LED灯
    P6OUT &= ~BIT4;

    P7DIR |= BIT4;         // 设置P8.1口为输出模式  控制LED灯
    P7OUT &= ~BIT4;        // 选中P8.1为输出方式
    P3DIR |= BIT5;         // 设置P8.1口为输出模式  控制LED灯
    P3OUT &= ~BIT5;        // 选中P8.1为输出方式
}

int apInterface_Key(void) { return 0; }

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer
    IO_Init();

    ADInit();

    //    TA0CTL   |= MC_1 + TASSEL_2 + TACLR;        //时钟为SMCLK,比较模式，开始时清零计数器
    //    TA0CCTL0  = CCIE;                           //比较器中断使能
    //    TA0CCR0   = 50000;                          //比较值设为50000，相当于50ms的时间间隔
    //    __enable_interrupt();

    volatile unsigned int ivalue = 0;        //设置判断变量
    while (1) {
        //  ivalue = GetAD();                                            //没有软件滤波
        ivalue = Filter();        //软件滤波
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
}
//#pragma vector = TIMER0_A0_VECTOR
//__interrupt void Timer_A(void) {
//    icnt++;
//    if (icnt == 4) {        //10s关灯
//        P1OUT &= ~BIT0;
//        icnt   = 0;
//    }
//}
