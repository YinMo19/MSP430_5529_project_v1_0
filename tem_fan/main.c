#include <msp430.h>

void main(void)
{
    // ֹͣ���Ź���ʱ��
    WDTCTL = WDTPW + WDTHOLD;

    // ����P2.4Ϊ���
    P2DIR |= BIT4;
    P2OUT |= BIT4; // ����P2.4Ϊ�ߵ�ƽ

    while (1)
    {
        // �ڴ˴���������������룬���߱��ֿ���
    }
}
