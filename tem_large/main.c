#include <msp430.h>
void ClockInit()
{        // ����MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ
    UCSCTL6 &= ~XT1OFF;   // ����XT1
    P5SEL |= BIT2 + BIT3; // XT2���Ź���ѡ��
    UCSCTL6 &= ~XT2OFF;   // ��XT2
    __bis_SR_register(SCG0);
    UCSCTL0 = DCO0 + DCO1 + DCO2 + DCO3 + DCO4;
    UCSCTL1 = DCORSEL_4;  // DCOƵ�ʷ�Χ��28.2MHZ����
    UCSCTL2 = FLLD_5 + 1; // D=16��N=1

    // n=8,FLLREFCLKʱ��ԴΪXT2CLK��
    // DCOCLK=D*(N+1)*(FLLREFCLK/n);
    // DCOCLKDIV=(N+1)*(FLLREFCLK/n);
    UCSCTL3 = SELREF_5 + FLLREFDIV_3;
    // ACLK��ʱ��ԴΪDCOCLKDIV,
    // MCLK\SMCLK��ʱ��ԴΪDCOCLK
    UCSCTL4 = SELA_4 + SELS_3 + SELM_3;
    // ACLK��DCOCLKDIV��32��Ƶ�õ���
    // SMCLK��DCOCLK��2��Ƶ�õ�
    UCSCTL5 = DIVA_5 + DIVS_1;
}

void ADInit() {
    ADC12CTL0  |= ADC12MSC;             //�Զ�ѭ������ת��
    ADC12CTL0  |= ADC12ON;              //����ADC12ģ��
    ADC12CTL1  |= ADC12CONSEQ_3;        //ѡ������ͨ�����ѭ������ת��
    ADC12CTL1  |= ADC12SHP;             //��������ģʽ
    ADC12CTL1  |= ADC12CSTARTADD_0;
    ADC12MCTL0 |= ADC12INCH_1;        //ͨ��ѡ��
    ADC12MCTL1 |= ADC12INCH_2 + ADC12EOS;
    ADC12CTL0  |= ADC12ENC;
}

#define N_POINT 13
unsigned int GetAD()
{
    unsigned int temp = 0;         //���ñ���
    ADC12CTL0 |= ADC12SC;   //��ʼ����ת��
    temp = ADC12MEM0; //�ѽ����������
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
    //���Ϊ2��n�η��ȷ֣��������>>n�ļ��ټ�����  a=a*8;  ����Ϊ a=a<<3;
    //b=b/8; ����Ϊb=b>>3;
    return (unsigned int)(sum / (N_POINT - 2));
}

unsigned int GetAD1()
{
    unsigned int temp = 0;         //���ñ���
    ADC12CTL0 |= ADC12SC;   //��ʼ����ת��
    temp = ADC12MEM1; //�ѽ����������
    return temp;
}

unsigned int icnt;
void IO_Init(void)
{

    P1DIR |= BIT0; // ����P3.6��Ϊ���ģʽ  ���Ʒ�����
    P1OUT |= BIT0; // ѡ��P3.6Ϊ�����ʽ

    P2DIR &= ~(BIT3 + BIT6);
    P2REN |= BIT3 + BIT6;
    P2OUT |= BIT3 + BIT6;

    P1DIR &= ~(BIT3 + BIT2);
    P1REN = BIT3 + BIT2;
    P1OUT |= BIT3 + BIT2;

    P1DIR |= BIT5;    // P1.2 ����Ϊ���
    P1OUT |= BIT5;
    P2DIR |= (BIT4 + BIT5);
    P2OUT &= ~BIT4;
    P2SEL |= BIT5;

}

int apInterface_Key(void)
{
    return 0;
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD; // stop watchdog timer
    IO_Init();
    ClockInit();
    int i = 0;
    ADInit();

    TA0CTL |= MC_1 + TASSEL_2 + TACLR; //ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
    TA0CCTL0 = CCIE;                   //�Ƚ����ж�ʹ��
    TA0CCR0 = 50000; //�Ƚ�ֵ��Ϊ50000���൱��50ms��ʱ����

    TA2CTL = TASSEL_2 + TACLR + MC_1;
    TA2CCR0 = 8000; // PWM����
    TA2CCTL2 = OUTMOD_7; // ���ģʽ7

    __enable_interrupt();

    volatile unsigned int ivalue = 0; //�����жϱ���

    while (1)
    {
        //  ivalue = GetAD();                                            //û������˲�
        ivalue = Filter(); //����˲�

        if (ivalue >= 2550&&ivalue < 2600)
        {
            for (i = 0; i < 8000; i++)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.0001);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.0001);
            }
            __delay_cycles(16000000 * 0.1);

        }
        else if(ivalue >= 2600&&ivalue < 2650 ){
            for (i = 0; i < 8000; i++)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.00005);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.00005);
            }
            __delay_cycles(16000000 * 0.1);

        }
        else if(ivalue >= 2650&&ivalue < 2700 ){
            for (i = 0; i < 8000; i++)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.000001);
            }
            __delay_cycles(16000000 * 0.1);
            for (i = 8000; i > 0; i--)
            {
                TA2CCR2 = i;
                __delay_cycles(16000000 * 0.000001);
            }
            __delay_cycles(16000000 * 0.1);

        }
        else if(ivalue >= 2700 ){
            TA2CCR2 = 8000;
        }
        else
        {
            TA2CCR2 = 0;

        }
    }
}
#pragma vector = TIMER0_A0_VECTOR
    __interrupt
    void Timer_A(void)
    {
        icnt++;
        if (icnt == 40)
        { //10s�ص�
            P1OUT &= ~BIT0;
            icnt = 0;
        }
    }
