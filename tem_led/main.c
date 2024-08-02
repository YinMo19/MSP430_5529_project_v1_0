#include <msp430.h>

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
unsigned int GetAD() {
    unsigned int temp  = 0;                //���ñ���
    ADC12CTL0         |= ADC12SC;          //��ʼ����ת��
    temp               = ADC12MEM0;        //�ѽ����������
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

unsigned int GetAD1() {
    unsigned int temp  = 0;                //���ñ���
    ADC12CTL0         |= ADC12SC;          //��ʼ����ת��
    temp               = ADC12MEM1;        //�ѽ����������
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
    //���Ϊ2��n�η��ȷ֣��������>>n�ļ��ټ�����  a=a*8;  ����Ϊ a=a<<3;
    //b=b/8; ����Ϊb=b>>3;
    return (unsigned int)(sum / (N_POINT - 2));
}

unsigned int icnt;
void         IO_Init(void) {
    P8DIR |= BIT1;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P8OUT &= ~BIT1;        // ѡ��P8.1Ϊ�����ʽ
    P3DIR |= BIT7;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P3OUT &= ~BIT7;        // ѡ��P8.1Ϊ�����ʽ
//
//    P1DIR |= BIT0;        // ����P3.6��Ϊ���ģʽ  ���Ʒ�����
//    P1OUT |= BIT0;        // ѡ��P3.6Ϊ�����ʽ

//    P2DIR &= ~(BIT3 + BIT6);
//    P2REN |= BIT3 + BIT6;
//    P2OUT |= BIT3 + BIT6;
//
//    P1DIR &= ~(BIT3 + BIT2);
//    P1REN  = BIT3 + BIT2;
//    P1OUT |= BIT3 + BIT2;

    P6DIR |= BIT3;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P6OUT &= ~BIT3;        // ѡ��P8.1Ϊ�����ʽ
    P6DIR |= BIT4;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P6OUT &= ~BIT4;

    P7DIR |= BIT4;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P7OUT &= ~BIT4;        // ѡ��P8.1Ϊ�����ʽ
    P3DIR |= BIT5;         // ����P8.1��Ϊ���ģʽ  ����LED��
    P3OUT &= ~BIT5;        // ѡ��P8.1Ϊ�����ʽ
}

int apInterface_Key(void) { return 0; }

/**
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;        // stop watchdog timer
    IO_Init();

    ADInit();

//    TA0CTL   |= MC_1 + TASSEL_2 + TACLR;        //ʱ��ΪSMCLK,�Ƚ�ģʽ����ʼʱ���������
//    TA0CCTL0  = CCIE;                           //�Ƚ����ж�ʹ��
//    TA0CCR0   = 50000;                          //�Ƚ�ֵ��Ϊ50000���൱��50ms��ʱ����
//    __enable_interrupt();

    volatile unsigned int ivalue  = 0;        //�����жϱ���
    while (1) {
        //  ivalue = GetAD();                                            //û������˲�
        ivalue  = Filter();        //����˲�
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
//    if (icnt == 4) {        //10s�ص�
//        P1OUT &= ~BIT0;
//        icnt   = 0;
//    }
//}
