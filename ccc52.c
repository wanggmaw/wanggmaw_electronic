#include <reg52.h>
#include "intrins.h" //???
#define uint unsigned int
#define uchar unsigned char
#define ulong unsigned long
#define PI 3.1415926

uchar table2[16] = "mode=";
uchar table3[16] = "R(om)=";
uchar table4[16] = "C(pF)=";
uchar table5[16] = "L(mH)=";

//mode:????,timer_flag:?????
uchar num, mode, th0, tl0, timer_flag;

//???????
bit range_flag;

ulong C, L;
ulong R;
double f;

sbit lcden = P3 ^ 6; //?????
sbit lcdrs = P3 ^ 7; //?????????
sbit RW = P3 ^ 5;    //1602???
sbit key_R = P0 ^ 0; //??????
sbit key_C = P0 ^ 1; //??????
sbit key_L = P0 ^ 2; //??????

sbit R_out = P1 ^ 0; //????????
sbit C_out = P1 ^ 1; //????????
sbit L_out = P1 ^ 2; //????????

sbit R_range_flag = P3 ^ 0;
sbit C_range_flag = P3 ^ 1;

//?????
void delayms(uint xms);      //????
void write_com(uchar com);   //???????
void write_data(uchar date); //???????
void led_init();             //???????
void t_init();               //???0?????
void keyscan();              //??????(???????????????)
void display_f(ulong f);     //??????
void display_R(ulong R);     //??????
void display_C(uint C);      //??????
void display_L(uint L);
void timer(); //??????

//???
void main()
{
    RW = 0;
    range_flag = 0;
    R_range_flag = 0;
    C_range_flag = 0;
    led_init();
    t_init();
    keyscan();
    mode = 4;
    timer_flag = 0;
    while (1)
    {
        keyscan();
        if (timer_flag == 1)
            timer();
        switch (mode)
        {
        case 1:
			if(!range_flag){
            	R = (ulong)((50000000.0/0.6931472/f - 50.0)*0.92);
				if(R>3000000){
					range_flag=1;
					R_range_flag=1;
					break;
				}
			}
			if(range_flag==1){
				R=(ulong)((500000000.0/0.6931472/f-50.0)*1.05);
				range_flag=1;
			}
			
			
            display_R(R);
            display_f(mode);
			
            timer_flag = 0;
            break;
        case 2:
			if(!range_flag){
				C = (int)((100000000.0 /551/ 0.6931472 / f + 0.5)*1.55);
				if(C>13000){
					range_flag=1;
					C_range_flag=1;
					break;
				}
			}
			if(range_flag==1){
				C=(int)(100000000.0 / 153 / 0.6931472 / f + 0.5);
				range_flag=1;
			}

            display_C(C);
            display_f(mode);
            timer_flag = 0;
            break;
        case 3:
            L = (ulong)(1000000.0/f*12/10/1.2333*0.8824*0.952);
            display_L(L);
            display_f(mode);
			timer_flag = 0;
            break;
        default:
            display_f(mode);
            break;
        }
    }
}
void timer()
{
    switch (mode)
    {
    case 1:
        while (R_out)
            ;
        while (!R_out)
            ;
        TH0 = 0;
        TL0 = 0;
        TR0 = 1;
        while (R_out)
            ;
        while (!R_out)
            ;
        th0 = TH0;
        tl0 = TL0;
        TR0 = 0;
        TH0 = 0;
        TL0 = 0;
        break;

    case 2:
        while (C_out)
            ;
        while (!C_out)
            ;
        TH0 = 0;
        TL0 = 0;
        TR0 = 1;
        while (C_out)
            ;
        while (!C_out)
            ;
        th0 = TH0;
        tl0 = TL0;
        TR0 = 0;
        TH0 = 0;
        TL0 = 0;
        break;
    case 3:
        while (L_out)
            ;
        while (!L_out)
            ;
        TH0 = 0;
        TL0 = 0;
        TR0 = 1;
        while (L_out)
            ;
        while (!L_out)
            ;
        th0 = TH0;
        tl0 = TL0;
        TR0 = 0;
        TH0 = 0;
        TL0 = 0;
        break;
    }
    f = 1000000.0 / (th0 * 256 + tl0);
    TR0 = 0;
}
//????
void T0_count() interrupt 1
{

    TH0 = 0;
    TL0 = 0;
    range_flag = 1;
    R_range_flag = 1;
    C_range_flag = 1;
}

//????
void delayms(uint xms)
{
    uint i, j;
    for (i = xms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

//???????
void write_com(uchar com)
{
    delayms(2);

    lcdrs = 0;
    P2 = com;

    lcden = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    lcden = 0;
}

//???????
void write_data(uchar date)
{
    delayms(2);
    lcdrs = 1;
    lcden = 1;
    P2 = date;
    lcden = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    lcden = 0;
}

//???????
void led_init()
{
    write_com(0x38); //??16×2??,5×7??,8?????
    write_com(0x0c);
    write_com(0x06);
    write_com(0x01);
}
//???0?????
void t_init()
{
    TMOD = 0x01; //?????0????1(M1M0=0x0001)
    TH0 = 0;     //???
    TL0 = 0;
    EA = 1;  //????
    ET0 = 1; //????0??
             //?????0
}

//??????(???????????????)
void keyscan()
{
    if (key_R == 0)
    {
        delayms(10);
        if (key_R == 0)
        {
            mode = 1;
            timer_flag = 1;
            range_flag = 0;
            R_range_flag = 0;
            C_range_flag = 0;
        }
    }
    if (key_C == 0)
    {
        delayms(10);
        if (key_C == 0)
        {
            mode = 2;
            timer_flag = 1;
            range_flag = 0;
            R_range_flag = 0;
            C_range_flag = 0;
        }
    }
    if (key_L == 0)
    {
        delayms(10);
        if (key_L == 0)
		{
            mode = 3;
        	timer_flag = 1;
       		range_flag = 0;
        	R_range_flag = 0;
        	C_range_flag = 0;
		}
    }
    //??????????
}
//??????
void display_f(ulong f)
{
    uchar count = 0;
    ulong f0;
    f0 = f;
    while (f)
    {
        f = f / 10;
        count++;
    }
    for (num = 4 + count; num > 4; num--)
    {
        table2[num] = f0 % 10 + 48;
        f0 = f0 / 10;
    }
    write_com(0x80);
    for (num = 0; num < 5 + count; num++)
    {
        write_data(table2[num]);
        delayms(5);
    }
}

//??????
void display_R(ulong R)
{
    uchar count = 0;
    ulong R0;
    R0 = R;
    while (R)
    {
        R = R / 10;
        count++;
    }
    for (num = 5 + count; num > 5; num--)
    {
        table3[num] = R0 % 10 + 48;
        R0 = R0 / 10;
    }
    write_com(0x80 + 0x40);
    for (num = 0; num < 16; num++)
    {
        if (num < 6 + count)
            write_data(table3[num]);
        else
            write_data(0x20);
        delayms(5);
    }
}

//??????
void display_C(ulong C)
{
    uchar count = 0;
    uint C0;
    C0 = C;
    while (C)
    {
        C = C / 10;
        count++;
    }
    for (num = 5 + count; num > 5; num--)
    {
        table4[num] = C0 % 10 + 48;
        C0 = C0 / 10;
    }
    write_com(0x80 + 0x40);
    for (num = 0; num < 16; num++)
    {
        if (num < 6 + count)
            write_data(table4[num]);
        else
            write_data(0x20);
        delayms(5);
    }
}

void display_L(ulong L)
{
    uchar count = 0;
    uint L0;
    L0 = L;
    while (L)
    {
        L = L / 10;
        count++;
    }
    for (num = 5 + count; num > 5; num--)
    {
        table5[num] = L0 % 10 + 48;
        L0 = L0 / 10;
    }
    write_com(0x80 + 0x40);
    for (num = 0; num < 16; num++)
    {
        if (num < 6 + count)
            write_data(table5[num]);
        else
            write_data(0x20);
        delayms(5);
    }
}
