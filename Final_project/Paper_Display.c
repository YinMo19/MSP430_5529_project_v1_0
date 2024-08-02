#include <msp430.h>
#include "Include.h"
#include "Paper_Display.h"

#define DELAY_TIME                 50        // 图片显示完停留时间(单位:秒)
// 测试图
#define PIC_WHITE                  255        // 全白
#define PIC_BLACK                  254        // 全黑
#define PIC_Orientation            253        // 方向图
#define PIC_LEFT_BLACK_RIGHT_WHITE 249        // 左黑右白
#define PIC_UP_BLACK_DOWN_WHITE    248        // 上黑下白

const unsigned char init_data[] = {
    0x50, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,

};

void initClock() {
    UCSCTL6 &= ~XT1OFF;            //启动XT1
    P5SEL   |= BIT2 + BIT3;        //XT2引脚功能选择
    UCSCTL6 &= ~XT2OFF;            //打开XT2
    __bis_SR_register(SCG0);
    UCSCTL0 = DCO0 + DCO1 + DCO2 + DCO3 + DCO4;
    UCSCTL1 = DCORSEL_4;         //DCO频率范围在28.2MHZ以下
    UCSCTL2 = FLLD_5 + 1;        //D=16，N=1
    UCSCTL3 =
        SELREF_5 +
        FLLREFDIV_3;        //n=8,FLLREFCLK时钟源为XT2CLK；DCOCLK=D*(N+1)*(FLLREFCLK/n);DCOCLKDIV=(N+1)*(FLLREFCLK/n);
    UCSCTL4 = SELA_4 + SELS_3 + SELM_3;        //ACLK的时钟源为DCOCLKDIV,MCLK\SMCLK的时钟源为DCOCLK
    UCSCTL5 = DIVA_5 + DIVS_1;        //ACLK由DCOCLKDIV的32分频得到，SMCLK由DCOCLK的2分频得到
                                      //最终MCLK:16MHZ,SMCLK:8MHZ,ACLK:32KHZ

    //	 __bic_SR_register(SCG0);                   //Enable the FLL control loop
}

void MyRESET() {
    nRST_L;
    DELAY_mS(10);        //1ms
    nRST_H;
    DELAY_mS(10);        //1ms
}
void DELAY_100nS(unsigned int delaytime)        // 30us
{
    int i, j;
    for (i = 0; i < delaytime; i++)
        for (j = 0; j < 10; j++)
            ;
}

void DELAY_mS(unsigned int delaytime)        // 1ms
{
    int i;
    for (i = 0; i < delaytime; i++)
        __delay_cycles(200);
}
void DELAY_S(unsigned int delaytime)        //  1s
{
    int i;
    for (i = 0; i < delaytime; i++) {
        __delay_cycles(3276);
    }
}
void DELAY_M(unsigned int delaytime)        //  1M
{
    int i;
    for (i = 0; i < delaytime; i++)
        DELAY_S(60);
}
void READBUSY() {
    while (1) {        //=1 BUSY
        if ((P2IN & BIT2) == 0)
            break;
    }
    DELAY_M(2);
}
void FIRST_PICTURE() {
    SPI4W_WRITECOM(0x21);
    SPI4W_WRITEDATA(0x83);
    SPI4W_WRITECOM(0x22);
    SPI4W_WRITEDATA(0xC4);
}
void INIT_SSD1673() {
    MyRESET();
    READBUSY();
    SPI4W_WRITECOM(0x01);         // Gate Setting
    SPI4W_WRITEDATA(0xF9);        // MUX Gate lines=250-1=249(F9H)
    SPI4W_WRITEDATA(
        0x00);        // B[2]:GD=0[POR](G0 is the 1st gate output channel)  B[1]:SM=0[POR](left and right gate interlaced)  B[0]:TB=0[POR](scan from G0 to G319)
    SPI4W_WRITECOM(0x3A);         // number of dummy line period   set dummy line for 50Hz frame freq
    SPI4W_WRITEDATA(0x06);        // Set 50Hz   A[6:0]=06h[POR] Number of dummy line period in term of TGate
    SPI4W_WRITECOM(0x3B);         // Gate line width   set gate line for 50Hz frame freq
    SPI4W_WRITEDATA(0x0B);        // A[3:0]=1011(78us)  Line width in us   78us*(250+6)=19968us=19.968ms
    SPI4W_WRITECOM(0x3C);         // Select border waveform for VBD
    //    SPI4W_WRITEDATA(0x30);    // GS0-->GS0
    //    SPI4W_WRITEDATA(0x31);    // GS0-->GS1
    //    SPI4W_WRITEDATA(0x32);    // GS1-->GS0
    SPI4W_WRITEDATA(0x33);        // GS1-->GS1  开机第一次刷新Border从白到白
    //    SPI4W_WRITEDATA(0x43);    // VBD-->VSS
    //    SPI4W_WRITEDATA(0x53);    // VBD-->VSH
    //    SPI4W_WRITEDATA(0x63);    // VBD-->VSL
    //    SPI4W_WRITEDATA(0x73);    // VBD-->HiZ

    SPI4W_WRITECOM(0x11);         // Data Entry mode
    SPI4W_WRITEDATA(0x01);        // 01 –Y decrement, X increment
    SPI4W_WRITECOM(0x44);         // set RAM x address start/end, in page 22
    SPI4W_WRITEDATA(0x00);        // RAM x address start at 00h;
    SPI4W_WRITEDATA(0x0f);        // RAM x address end at 0fh(15+1)*8->128    2D13
    SPI4W_WRITECOM(0x45);         // set RAM y address start/end, in page 22
    SPI4W_WRITEDATA(0xF9);        // RAM y address start at FAh-1;		    2D13
    SPI4W_WRITEDATA(0x00);        // RAM y address end at 00h;		    2D13

    SPI4W_WRITECOM(0x2C);        // Vcom= *(-0.02)+0.01???
    //    SPI4W_WRITEDATA(0x82);    //-2.5V
    //    SPI4W_WRITEDATA(0x69);    //-2V
    SPI4W_WRITEDATA(0x4B);        //-1.4V
    //    SPI4W_WRITEDATA(0x50);    //-1.5V
    //    SPI4W_WRITEDATA(0x37);    //-1V
    //    SPI4W_WRITEDATA(0x1E);    //-0.5V

    WRITE_LUT();
    SPI4W_WRITECOM(0x21);         // Option for Display Update
    SPI4W_WRITEDATA(0x83);        // A[7]=1(Enable bypass)  A[4]=0全黑(value will be used as for bypass)
    DIS_IMG(PIC_WHITE);           // 全黑到全白清屏，这样可防止开机出现花屏的问题

    SPI4W_WRITECOM(0x21);         //
    SPI4W_WRITEDATA(0x03);        // 后面刷新恢复正常的前后2幅图比较
    SPI4W_WRITECOM(0x3C);         // Select border waveform for VBD
    SPI4W_WRITEDATA(0x73);        // VBD-->HiZ  后面刷新时Border都是高阻
}

void WRITE_LUT() {
    unsigned char i;
    SPI4W_WRITECOM(0x32);        //write LUT register
    for (i = 0; i < 29; i++)
        SPI4W_WRITEDATA(init_data[i]);        //write LUT register
}

void Init_buff(void) {
    int i;
    for (i = 0; i < 4000; i++) {
        DisBuffer[i] = gImage_logo[i];
    }
}

// void Init_buff_2(void) {
//     int i;
//     for (i = 0; i < 4000; i++) {
//         DisBuffer[i] = gImage_b[i];
//     }
// }

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xx   图片显示函数    xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void DIS_IMG(unsigned char num) {
    unsigned int row, col;
    unsigned int pcnt;

    SPI4W_WRITECOM(0x4E);
    SPI4W_WRITEDATA(0x00);        // set RAM x address count to 0;
    SPI4W_WRITECOM(0x4F);
    SPI4W_WRITEDATA(0xF9);        // set RAM y address count to 250-1;	2D13
    DELAY_S(5);
    SPI4W_WRITECOM(0x24);
    DELAY_S(5);
    pcnt = 0;                              // 复位或保存提示字节序号
    for (col = 0; col < 250; col++)        // 总共250 GATE列  // send 128x250bits ram 2D13
    {
        for (row = 0; row < 16; row++)        // 总共128 SOURCE行，每个像素1bit,即 128/8=16 字节
        {
            switch (num) {
            case 1:
                SPI4W_WRITEDATA(DisBuffer[pcnt]);
                break;
            case 2:
                SPI4W_WRITEDATA(gImage_logo[pcnt]);
                break;
            case PIC_WHITE:
                SPI4W_WRITEDATA(0xff);
                break;
            case 3:
                SPI4W_WRITEDATA(gImage_b[pcnt]);
                break;
            default:
                break;
            }
            pcnt++;
        }
    }
    SPI4W_WRITECOM(0x22);
    SPI4W_WRITEDATA(
        0xC7);        // (Enable Clock Signal, Enable CP) (Display update,Disable CP,Disable Clock Signal)
    //  SPI4W_WRITEDATA(0xF7);    // (Enable Clock Signal, Enable CP, Load Temperature value, Load LUT) (Display update,Disable CP,Disable Clock Signal)
    SPI4W_WRITECOM(0x20);
    DELAY_mS(1);
    READBUSY();
    DELAY_S(DELAY_TIME);
}

void SetpointXY(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye) {
    SPI4W_WRITECOM(0x44);        //set RAM x address start/end, in page 36
    SPI4W_WRITEDATA(xs);         //RAM x address start at 00h;
    SPI4W_WRITEDATA(xe);         //RAM x address end at 11h(17)->72: [because 1F(31)->128 and 12(18)->76]

    SPI4W_WRITECOM(0x45);        //set RAM y address start/end, in page 37
    SPI4W_WRITEDATA(ys);         //RAM y address start at 00h;
    SPI4W_WRITEDATA(ye);         //RAM y address start at ABh(171)->172: [because B3(179)->180]

    SPI4W_WRITECOM(0x4E);        //set RAM x address count to 0;
    SPI4W_WRITEDATA(xs);
    SPI4W_WRITECOM(0x4F);        //set RAM y address count to 0;
    SPI4W_WRITEDATA(ye);

    SPI4W_WRITECOM(0x24);
}

void enterdeepsleep() {
    SPI4W_WRITECOM(0x10);
    SPI4W_WRITEDATA(0x01);
}

void SPI4W_WRITECOM(unsigned char INIT_COM) {
    unsigned char TEMPCOM;
    unsigned char scnt;
    TEMPCOM = INIT_COM;
    nCS_H;
    nCS_L;
    SCLK_L;
    nDC_L;
    for (scnt = 0; scnt < 8; scnt++) {
        if (TEMPCOM & 0x80)
            SDA_H;
        else
            SDA_L;
        DELAY_100nS(10);
        SCLK_H;
        DELAY_100nS(10);
        SCLK_L;
        TEMPCOM = TEMPCOM << 1;
        DELAY_100nS(10);
    }
    nCS_H;
}

void SPI4W_WRITEDATA(unsigned char INIT_DATA) {
    unsigned char TEMPCOM;
    unsigned char scnt;
    TEMPCOM = INIT_DATA;
    nCS_H;
    nCS_L;
    SCLK_L;
    nDC_H;
    for (scnt = 0; scnt < 8; scnt++) {
        if (TEMPCOM & 0x80)
            SDA_H;
        else
            SDA_L;
        DELAY_100nS(10);
        SCLK_H;
        DELAY_100nS(10);
        SCLK_L;
        TEMPCOM = TEMPCOM << 1;
        DELAY_100nS(10);
    }
    nCS_H;
}

unsigned char FontSize[] = {16, 32};

/**
 * @brief 
 * 
 * @param str 指向待显示的字符串
 * @param xsize 字符在X方向上的位置
 * @param ysize 字符在Y方向上的位置
 * @param font 字体类型，可选值为0,1,2
 * @param size 字号大小，可选值为0,1
 * @param reverse 是否反显，0为正常显示，1为反显
 * @param fresh 是否立即刷新显示
 */
void          display(unsigned char *str,
                      unsigned int   xsize,
                      unsigned int   ysize,
                      unsigned int   font,
                      unsigned int   size,
                      unsigned int   reverse,
                      unsigned int   fresh) {
    // 循环变量
    unsigned char          i, j, k;
    // 指向字模的指针
    unsigned char         *zimoPoint;
    // 字模宽度（点数）
    unsigned char          zimoW;
    // 字模高度（点数）
    unsigned char          zimoH;
    // 需要移动的字节数
    unsigned char          moveBytes;
    // 需要移动的位数
    unsigned char          moveBits;
    // 临时存储字模数据的数组
    volatile unsigned char zimoBufferTemp[6];

    // 根据字号计算字模的宽度和高度
    zimoW    = FontSize[size] / 2;
    zimoH    = FontSize[size] / 8;

    // 计算因ysize非8的倍数而需要额外移动的位数
    moveBits = ysize % 8;
    if (moveBits != 0) {
        // 如果有余数，说明需要额外移动一个字节
        moveBytes = zimoH + 1;
    } else {
        moveBytes = zimoH;
    }

    // 遍历字符串中的每个字符
    while (*str) {
        // 根据字体和字号选择相应的字模
        if ((font == TimesNewRoman) && (size == size8))
            zimoPoint = (unsigned char *)&TimesNewRoman8;
        else if ((font == TimesNewRoman) && (size == size16))
            zimoPoint = (unsigned char *)&TimesNewRoman16;
        // 计算字符在字模中的起始地址
        zimoPoint = zimoPoint + ((*str) - 0x20) * zimoW * zimoH;

        // 遍历字模的每一列
        for (i = 0; i < zimoW; i++) {
            // 初始化临时字模缓冲区
            for (j = 0; j < 6; j++)
                zimoBufferTemp[j] = 0;

            // 根据是否反显，读取字模数据并取反或保持原样
            if (reverse == 0) {
                for (j = 0; j < zimoH; j++) {
                    zimoBufferTemp[j] = ~(*(zimoPoint + i + j * zimoW));
                }
            } else {
                for (j = 0; j < zimoH; j++) {
                    zimoBufferTemp[j] = (*(zimoPoint + i + j * zimoW));
                }
            }

            // 对字模数据进行位移操作
            if (moveBits == 0) {
                // 如果不需要移动则跳过
            } else {
                for (k = 0; k < moveBits; k++) {
                    for (j = moveBytes; j > 0; j--) {
                        zimoBufferTemp[j] = zimoBufferTemp[j] >> 1;
                        if ((zimoBufferTemp[j - 1] & 0x01) > 0)
                            zimoBufferTemp[j] = zimoBufferTemp[j] + 0x80;
                    }
                    zimoBufferTemp[0] = zimoBufferTemp[0] >> 1;
                }
            }

            // 处理显示缓冲区
            if (moveBits == 0) {
                // 如果不需要移动则跳过
            } else {
                k = (0xff >> moveBits);
                zimoBufferTemp[moveBytes - 1] =
                    zimoBufferTemp[moveBytes - 1] | (DisBuffer[(xsize + i) * 16 + ((ysize / 8) + zimoH)] & k);
                k = ~k;
                zimoBufferTemp[0] = zimoBufferTemp[0] | (DisBuffer[(xsize + i) * 16 + (ysize / 8)] & k);
            }

            // 更新显示缓冲区
            for (j = 0; j < moveBytes; j++) {
                DisBuffer[(xsize + i) * 16 + ((ysize / 8) + j)] = zimoBufferTemp[j];
            }
        }
        // 更新X方向上的位置并指向下一个字符
        xsize = xsize + zimoW;
        str++;
    }
}

void PaperIO_Int(void) {
    P1DIR |= BIT4;
    P2DIR |= BIT7;
    P2DIR &= ~BIT2;
    P3DIR |= BIT2 + BIT3 + BIT4;
}
