#include"STC12C2052.h"  
#include<intrins.h>  
#include "1602.h"
#define uchar unsigned char  
#define uint  unsigned int   
uchar data_buf[100]={0},start_recive=0,start_fram=0,frame_len=0,data_len=0;
uchar reboot_cnt=0;
unsigned short real_data[50]={0};

void reboot(uchar buf)
{
  if(buf==0x7f)//������������
  {
     reboot_cnt++;
	 if(reboot_cnt==30)
	 {
		reboot_cnt=0;
	    ISP_CONTR=0X60;//�Զ�����ϵͳ
	 }
   }
}
unsigned short get_crc1(uchar *ptr,uchar len)
{
  uchar i;
  unsigned short crc=0xFFFF;
  if(len==0) len=1;
  while(len--)
  {
    crc ^=*ptr;
    for(i=0;i<8;i++)
    {
      if(crc&1)
      {
        crc>>=1;
        crc ^= 0XA001; 
      }
      else crc>>=1;
    }
    ptr++;
  }
  return(crc);
}

unsigned short get_crc(uchar *ptr,uchar len)
{
  uchar i;
  unsigned short crc=0xFFFF;
  if(len==0) len=1;
  while(len--)
  {
    crc ^=*ptr;
    for(i=0;i<8;i++)
    {
      if(crc&1)
      {
        crc>>=1;
        crc ^= 0XA001; 
      }
      else crc>>=1;
    }
    ptr++;
  }
  return(crc);
}  


void UartInit(void)//ʹ�ö�ʱ��1ģʽ
{
		SCON = 0x50;//���ڹ�����ʽ1,������գ�SCON = 0x40ʱ��ֹ���գ�
		//PCON = 0x80;//������(��Ƶ)
		TMOD = 0x20;//��ʱ��1������ʽ2
		AUXR |= 0X40;//��ʱ��1������1Tģʽ��Ϊȫ��,(��Ϊ12Tģʽ)
	    //TH1 = 0xFA;	//115200	
		TH1 = 0xee;	//��ʱ����ֵ��8λ����38400
		TL1 = TH1;	//��ʱ����ֵ��8λ����
		EA = 1; //�������ж� 
		ES = 1; //����UART���ڵ��ж�
		TR1 = 1;	//��ʱ������    
}      

void send_data(uchar device_n,uchar reg_n,uchar reg_mun)
{
	  unsigned short crc;
	  uchar i=0;
      uchar read_buf[11]={0xaa,0x55,0xcc};
	  read_buf[3]=device_n;
	  read_buf[4]=0x03;
	  read_buf[5]=0x00;
	  read_buf[6]=reg_n;
	  read_buf[7]=0x00;
	  read_buf[8]=reg_mun;
	  crc=get_crc1(&read_buf[3],6);
	  read_buf[9]=crc&0xff;
	  read_buf[10]=(crc>>8)&0xff;
	  for(i=0;i<11;i++)
	  {
	     SBUF = read_buf[i];
		 while(TI == 0);
		 TI=0;
	  }
	
}      

//ָ����ʽ
void lcd_display(unsigned short *real_data)
{
   uchar disbuf[17]={0};
   disbuf[0]='v';
   disbuf[1]=(*(real_data)>>8)+48;
   disbuf[2]='.';
   disbuf[3]=(*(real_data)&0x00ff)+48;
   disbuf[4]='.';
   disbuf[5]=(*(real_data+1)>>8)+48;
   disbuf[6]=' ';
   disbuf[7]=' ';
   disbuf[8]=((*(real_data+1)&0x00ff)>>4)+48;
   disbuf[9]=(*(real_data+1)&0x000f)+48;
   disbuf[10]='-';
   disbuf[11]=(*(real_data+2)>>12)+48;
   disbuf[12]=((*(real_data+2)>>8)&0x000f)+48;
   disbuf[13]= '-';
   disbuf[14]=((*(real_data+2)>>4)&0x000f)+48;
   disbuf[15]=(*(real_data+2)&0x000f)+48;
   disbuf[16]= '\0';
   xiezfc(2,1,disbuf);
} 

/*������ʽ
void lcd_display(unsigned short *real_data)
{
   uchar disbuf[17]={0};
   disbuf[0]='v';
   disbuf[1]=(real_data[0]>>8)+48;
   disbuf[2]='.';
   disbuf[3]=(real_data[0]&0x00ff)+48;
   disbuf[4]='.';
   disbuf[5]=(real_data[1]>>8)+48;
   disbuf[6]=' ';
   disbuf[7]=' ';
   disbuf[8]=((real_data[1]&0x00ff)>>4)+48;
   disbuf[9]=(real_data[1]&0x000f)+48;
   disbuf[10]='-';
   disbuf[11]=(real_data[2]>>12)+48;
   disbuf[12]=((real_data[2]>>8)&0x000f)+48;
   disbuf[13]= '-';
   disbuf[14]=((real_data[2]>>4)&0x000f)+48;
   disbuf[15]=(real_data[2]&0x000f)+48;
   disbuf[16]= '\0';
   xiezfc(2,1,disbuf);
} */ 

void clear_real_data()
{
  uchar i;
  for(i=0;i<50;i++)
  {
    real_data[i]=0x0000;
  }

}  



void Main(void)  
{  
  uchar device=0;
  unsigned short disply_data[3]={0};
  LE=0;  
  init_1602();    
  UartInit();
  xiezfc(1,1,"<version-tester>");
  xiezfc(2,1,"    ---jack_chen");
  delay(1000);
  while(1)  
  {  
    //��Դ�����
    send_data(0x05,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=1;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //�����
    send_data(0x01,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=2;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //����
    send_data(0x06,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)
	{
	  device=3;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //����
    send_data(0x07,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=4;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //�����ư�
    send_data(0x03,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=5;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();

    //�ֺ����ư�
    send_data(0x04,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=6;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();	

    //����
    send_data(0x02,0x78,3);
    delay(30);
    if(real_data[0]!=0&&real_data[1]!=0&&real_data[2]!=0)	
	{
	  device=7;
	  disply_data[0]=real_data[0];
	  disply_data[1]=real_data[1];
	  disply_data[2]=real_data[2];
	}
	clear_real_data();
	
	if(device!=0)
	{
	  if(device==1)
	  {
	    xiezfc(1,1,"Power board     ");
		lcd_display(disply_data);		

	  }
	  if(device==2)
	  {
	    xiezfc(1,1,"Turn board   [1]");
		lcd_display(disply_data);
	  }
	  if(device==3)
	  {
	    xiezfc(1,1,"Left hand    [6]");
		lcd_display(disply_data);
	  }
	  if(device==4)
	  {
	    xiezfc(1,1,"Right hand   [7]");
		lcd_display(disply_data);
	  }
	  if(device==5)
	  {
	    xiezfc(1,1,"Head-LED     [3]");
		lcd_display(disply_data);
	  }
	  if(device==6)
	  {
	    xiezfc(1,1,"Hand-LED     [4]");
		lcd_display(disply_data);
	  }
	  if(device==7)
	  {
	    xiezfc(1,1,"Turn board   [2]");
		lcd_display(disply_data);
	  }

	  device=0;	
	  disply_data[0]=0;
	  disply_data[1]=0;
	  disply_data[2]=0;
	}
    else
    {
	   xiezfc(1,1,"Testing....     ");
	   xiezfc(2,1,"                ");
    }       	   

    delay(300);

  }
}
	
void get_data(void)	interrupt 4  
{
  uchar i,j;
  unsigned short crc_result,ck_crc;  
            
  if(RI == 1)   //��Ӳ�����յ�һ������ʱ��RI����λ	
  {
    RI = 0; 
    data_buf[data_len]=SBUF;
	reboot(data_buf[data_len]);
    if(start_recive==0)
    {
      if(data_len==0&&data_buf[data_len]==0xaa)
      {  
        data_len=1; 
      }      
	  else if(data_len==1&&data_buf[data_len]==0xaa)//���´δ����Ļ���֡ͷ�ٴ���aa��ʼ�ж�֡
      {  
       data_len=1;  
      } 
      else if(data_len==1&&data_buf[data_len]==0x55)
      {
        data_len=2;  
      }    
      else if(data_len==2&&data_buf[data_len]==0xcc)
      {  
       start_recive=1;
      }	
      else 
      {
       data_len=0;
      }
    }    
    if(start_recive==1)
    {
      if(start_fram==1)//���ճ�������
      {
        if(frame_len>=70)
        {
           start_recive=0;           
           start_fram=0;
		   data_len=0;
		   frame_len=0;
           return;
        }
        frame_len--;
        if(frame_len==0)
        {
           crc_result = get_crc(&data_buf[3],data_buf[5]+3);//����֡����CRC
           ck_crc = data_buf[ (data_buf[5]+7)]<<8 | data_buf[ (data_buf[5]+6) ];//�͸��ֽ�CRCת��Ϊ����
	       if(crc_result==ck_crc)//CRCУ��ȶ�
	       {  
	         for(i=0;i<(data_buf[5]/2);i++)//�������ݶԸ���
	         {
	           real_data[i]=data_buf[6+j]<<8| data_buf[7+j];//��Ч���ݺ�Ϊ2���ֽڵ����ݣ�һ���Ĵ���������ֵΪ2���ֽڣ� 
	           j=j+2;
	         }	         
           }            
           start_recive=0;
           start_fram=0;
		   data_len=0;
		   frame_len=0;
		   j=0; i=0;
           return;
        }
      }        
      if(data_len==5)
      {
        start_fram=1;
        frame_len=data_buf[5]+2;
      }
      data_len++;
    }
  }
}