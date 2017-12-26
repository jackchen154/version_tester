//#include"STC12C5A.h"
#include <intrins.h>
#define port P1
#define uchar unsigned char
sbit D7=port^7;
sbit D6=port^6;
sbit D5=port^5;
sbit D4=port^4;
sbit EN=port^3;
sbit RW=port^2;
sbit RS=port^1;
sbit LE=port^0;

bit lcd_busy();
void delay( int ms);
void write_com(uchar com);
void write_data(uchar date);
void init_1602(); 
void xiezfc(uchar hang,uchar lie,uchar *p);//�ַ���
void Delay1ms();



void delay( int ms)
{                           
   while(ms--)
   {
     Delay1ms();
   }

}

void Delay1ms()		//@22.1184MHz
{
	unsigned char i, j;

	i = 22;
	j = 128;
	do
	{
	  while (--j);
	} while (--i);
}

void write_com(uchar com)//дָ��
{
  while(lcd_busy());
  D7=com&0x80;
  D6=com&0x40;
  D5=com&0x20;
  D4=com&0x10;
  RS=0;
  RW=0;
  EN=0;
   
   
  EN=1;	        //�ȴ����4λ
   
   
  EN=0;
  D7=com&0x08;	 //�ٴ����4λ
  D6=com&0x04;
  D5=com&0x02;
  D4=com&0x01;
  RS=0;
  RW=0;
  EN=0;
   
   
  EN=1;		  
   
   
  EN=0;
  RS=0;
  RW=0;
  delay(5);

}


void write_data(uchar date)//д����
{
  while(lcd_busy());
  D7=date&0x80;
  D6=date&0x40;
  D5=date&0x20;
  D4=date&0x10;
  RS=1;
  RW=0;
  EN=0;
   
   
  EN=1;		   //ʹ�ܸ�4λ
   
   
  EN=0;
  D7=date&0x08;
  D6=date&0x04;
  D5=date&0x02;
  D4=date&0x01;
  RS=1;
  RW=0;
  EN=0;
   
   
  EN=1;		   //ʹ�ܵ�4λ
   
   
  EN=0;
  delay(5);
}

bit lcd_busy() //æµ��־λ
{                          
    bit result;
    RS = 0;
    RW = 1;
    EN = 1;
     
     
     result =D7;
    EN = 0;
    return result; 
}



void xiezfc(uchar hang,uchar lie,uchar *p)
                                      //��ָ����λ�÷����ַ���*PΪ�ַ���
{							       	  //����������׵�ַ
	uchar a;
	if(hang == 1) a = 0x80;
	if(hang == 2) a = 0xc0;
	a = a + lie - 1;
	 write_com(a);
	while(1)
	{
	
	
		if(*p == '\0') break;
		write_data(*p);
		p++;
	}
}

void init_1602()
{
  delay(10);//��ʱһ��50
  write_com(0x28);//4���ߣ�˫����ʾ��5x7�����ַ�
  write_com(0x28);
  write_com(0x0c);//00001DCB{D:��ʾ(1:��,0:��)
  				  //		 C:���(1:��ʾ,0:����)	 
  				  //		 B:�����(1:��,0:����)
				  //         }
  write_com(0x01);//����


}