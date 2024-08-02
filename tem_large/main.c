#include <msp430.h>
void ClockInit() {                 // 最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
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

unsigned int icnt;
void         IO_Init(void) {
    P1DIR |= BIT0;        // 设置P3.6口为输出模式  控制蜂鸣器
    P1OUT |= BIT0;        // 选中P3.6为输出方式

    P2DIR &= ~(BIT3 + BIT6);
    P2REN |= BIT3 + BIT6;
    P2OUT |= BIT3 + BIT6;

    P1DIR &= ~(BIT3 + BIT2);
    P1REN  = BIT3 + BIT2;
    P1OUT |= BIT3 + BIT2;

    P1DIR |= BIT5;        // P1.2 方向为输出
    P1OUT |= BIT5;
    P2DIR |= (BIT4 + BIT5);
    P2OUT &= ~BIT4;
    P2SEL |= BIT5;
}

int apInterface_Key(void) { return 0; }

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer
    IO_Init();
    ClockInit();
    int i = 0;
    ADInit();

    TA0CTL   |= MC_1 + TASSEL_2 + TACLR;        //时钟为SMCLK,比较模式，开始时清零计数器
    TA0CCTL0  = CCIE;                           //比较器中断使能
    TA0CCR0   = 50000;                          //比较值设为50000，相当于50ms的时间间隔

    TA2CTL    = TASSEL_2 + TACLR + MC_1;
    TA2CCR0   = 8000;            // PWM周期
    TA2CCTL2  = OUTMOD_7;        // 输出模式7

    __enable_interrupt();

    volatile unsigned int ivalue = 0;        //设置判断变量

    while (1) {
        //  ivalue = GetAD();                                            //没有软件滤波
        ivalue = Filter();        //软件滤波

        if (ivalue >= 2550 && ivalue < 2600) {
            for (i = 0; i < 8000; i++) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.0001);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.0001);
            }
            __delay_cycles(16000000 * 0.1);

        } else if (ivalue >= 2600 && ivalue < 2650) {
            for (i = 0; i < 8000; i++) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.00005);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.00005);
            }
            __delay_cycles(16000000 * 0.1);

        } else if (ivalue >= 2650 && ivalue < 2700) {
            for (i = 0; i < 8000; i++) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.000001);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--) {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.000001);
            }
            __delay_cycles(16000000 * 0.1);

        } else if (ivalue >= 2700) {
            TA2CCR2 = 8000;
        } else {
            TA2CCR2 = 0;
        }
    }
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A(void) {
    icnt++;
    if (icnt == 40) {        //10s关灯
        P1OUT &= ~BIT0;
        icnt   = 0;
    }
}
