int get_data(int fd)
{	
    uchar j=0,i=0; 
    read(fd,&sbuf,1);
    data_buf[data_len]=sbuf;
    //printf("%X\n",data_buf[data_len]);
    //printf("data_len:%d\n",data_len);
   if(start_recive==0)
   {
     if(data_len==0&&data_buf[data_len]==0xaa)
     {  
       data_len=1; 
       //printf("data_len is:%d\n",data_len);
     }  
     else if(data_len==1&&data_buf[data_len]==0xaa)//���´δ����Ļ���֡ͷ�ٴ���aa��ʼ�ж�֡
     {  
       data_len=1;  
       //printf("data_len is:%d\n",data_len);
     } 
     else if(data_len==1&&data_buf[data_len]==0x55)
     {
       data_len=2;  
       //printf("data_len is:%d\n",data_len);
     }    
     else if(data_len==2&&data_buf[data_len]==0xcc)
     {  
       start_recive=1;
       //printf("data_len is:%d\n",data_len);
     }	
     else 
     {
       data_len=0;
       //printf("data_len is:%d\n",data_len);
     }
   }
    
    if(start_recive==1)
    {
	if(start_fram==1)//���ճ�������
	{
          if(frame_len>=70)
          {
               //printf("the_len is too long ignore!\n");
               start_recive=0;
               data_len=0;
               start_fram=0;
               //menset(data_buf,0,100);
               return -1;
          }
          frame_len--;
          //printf("fram_len--:%d\n",frame_len);
          if(frame_len==0)
          {
              //printf("\ncrc start\n");
              crc_result = get_crc(&data_buf[3],data_buf[5]+3);//����֡����CRC
              ck_crc = data_buf[ (data_buf[5]+7)]<<8 | data_buf[ (data_buf[5]+6) ];//�͸��ֽ�CRCת��Ϊ����
	      //printf("crc_ok:%d\n",crc_result);
	      //printf("daice:%d\n",ck_crc);
              //printf("H is:%X,L is:%X\n",data_buf[ (data_buf[5]+7)],data_buf[ (data_buf[5]+6) ]);
	      if(crc_result==ck_crc)//CRCУ��ȶ�
	      {  
                 save_len=data_buf[5]/2;//�������ݶԸ���
                 //printf("crc check ok,save_len is: %d\n",save_len);
	         for(i=0;i<save_len;i++)
	         {
	            real_data[i]=data_buf[6+j]<<8| data_buf[7+j];//��Ч���ݺ�Ϊ2���ֽڵ����ݣ�һ���Ĵ���������ֵΪ2���ֽڣ� 
                    //printf("%X  ",real_data[i]);
	            j=j+2;
	         }
                 //printf("\n----------------------------------------------\n");
	         
               }            
               start_recive=0;
               data_len=0;
               start_fram=0;
               //menset(data_buf,0,100);
               return i;
          }
        }        
        if(data_len==5)
        {
           //printf("head detect: %X %X %X,\n",data_buf[0],data_buf[1],data_buf[2]);
           start_fram=1;
           frame_len=data_buf[5]+2;
           //printf("frame_len=%d\n",frame_len);
        }
       data_len++;
     }
    return 0;
}