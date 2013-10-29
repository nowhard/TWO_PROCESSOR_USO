#include "i2c.h"
#include "crc_table.h"//��� crc
#include "channels.h"
//-------------------------------------------------------------

volatile unsigned char xdata DEV_ADDR;

volatile unsigned char xdata byte_num_write=0;//���������� ���� � �������
volatile unsigned char xdata byte_num_read=0;//���������� ���� � �������(��� ��������� ������)
volatile unsigned char *i2c_buf;//

//volatile unsigned char xdata repeated_strt_read=0, i2c_address_write=0;
volatile unsigned char  temp_val=0x12;
volatile unsigned char i2c_mas[10]={0};


volatile struct I2C_Channel xdata i2c_channels;

unsigned char idata i2c_buffer[6]={0};

extern volatile unsigned char xdata  STATE_BYTE;
extern unsigned char channel_number;//���������� �������

//-------------------------------------------------------------
volatile unsigned char START_I2C=0;

volatile unsigned char ERROR_I2C=0;

struct pt pt_i2c_read_buf, pt_i2c_write_buf,pt_i2c_read_complete;

static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len));//�������� ����� ������ ������ I2C
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len));//�������� ����� ������ ������ I2C
static PT_THREAD(I2C_Read_Complete(struct pt *pt));//������������� ������
//-------------------------------------------------------------

//-------------------------------------------------------------

void I2C_Init(void)
{
	I2CM=1;//�������
	PT_INIT(&pt_i2c_read_buf);//������������� �������� �������
	PT_INIT(&pt_i2c_write_buf);
	PT_INIT(&pt_i2c_read_complete);

	return;
}
//--------------------------------------------------------------
void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len)//������ � ����� ��������
{


	DEV_ADDR=(addr<<1)&0xFE;

	
	byte_num_write=par_buf_len;
	byte_num_read=buf_len;
	
	i2c_buf=par_buf;
	//repeated_strt_read=1;
	START_I2C=1;//��������
	
	return;
}
//-------------------------------------------------------------
//#pragma OT(0,Speed) 
 PT_THREAD(I2C_Process(struct pt *pt))//�����  I2C
 {  
   PT_BEGIN(pt);

  while(1) 
  {
	 
	 PT_WAIT_UNTIL(pt,START_I2C);
	 START_I2C=0;

//	 //--------------start---------------
	 MDE=1;	//software output enable


	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	  			   
		 PT_YIELD(pt);//����� ������ ��������� �����	   
		 MDO=0;				   
	   //------------------------

	 //-------------�������� ������-------

	 PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1)); //�������� �����


	  if(ERROR_I2C)//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;				
			PT_YIELD(pt);//����� ������ ��������� �����										
			MDO=1;
		  //------------------------
		    ERROR_I2C=0;			
			PT_RESTART(pt);	
	  }
	  //PT_YIELD(pt);//����� ������ ��������� �����
  //--------------------------------------------------------------------------------------------------
	  
	  //i2c_buf[0]=0x8;

	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&i2c_buffer/*i2c_buf*/,byte_num_write)); //�������� ���������
	  if(ERROR_I2C)//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;				
			PT_YIELD(pt);//����� ������ ��������� �����										
			MDO=1;
		  //------------------------
		  	ERROR_I2C=0;	  		
			PT_RESTART(pt);	
	  }
	  //PT_YIELD(pt);//����� ������ ��������� �����
////------------------------------------------------------------------------------------------------------
	  //-----------��������� �����--------
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	   			   
		 PT_YIELD(pt);//����� ������ ��������� �����	   
		 MDO=0;				   
	   //------------------------
	  DEV_ADDR|=1;


	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1)); //�������� �����

	  if(ERROR_I2C)//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;	
			PT_YIELD(pt);//����� ������ ��������� �����										
			MDO=1;
		  //------------------------
		    ERROR_I2C=0;	  	
			PT_RESTART(pt);	
	  }
	 // PT_YIELD(pt);//����� ������ ��������� �����
 //---------------------------------------------------------------------------------------------------------------------
	  PT_SPAWN(pt, &pt_i2c_read_buf, I2C_Read_Buf(&pt_i2c_read_buf,&i2c_channels.I2C_CHNL.i2c_buf,byte_num_read)); //��������� ������
	  if(ERROR_I2C)//������� ������
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;					
			PT_YIELD(pt);//����� ������ ��������� �����									
			MDO=1;
		  //------------------------
		  	ERROR_I2C=0;	  		
			PT_RESTART(pt);	
	  }
	 // PT_YIELD(pt);//����� ������ ��������� �����

	  //-------I2C STOP--------
		MCO=0;
	    MDO=0;	  				
		MCO=1;
		PT_YIELD(pt);//����� ������ ��������� �����									
		MDO=1;

	  //-----------------------------------------------------------------------------

	  PT_SPAWN(pt, &pt_i2c_read_complete, I2C_Read_Complete(&pt_i2c_read_complete));

	 //----------------------------------
  }
  PT_END(pt);

 }
//-------------------------------------------------------------
//#pragma OT(0,Speed)
 static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len))//�������� ����� ������ ������ I2C
 {  
	    static volatile unsigned char read_byte_counter,bit_counter,recieve_byte;
	   
	  PT_BEGIN(pt);
	  
	  read_byte_counter=0;

	  while(read_byte_counter<len) //���� ������ ������
	  {	
			recieve_byte=0;
			MDE=0;//����� 
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			for(bit_counter=0;bit_counter<8;bit_counter++) //���� ������ �����
			{
				MCO=1;

				//PT_YIELD(pt);//����� ������ ��������� �����
				recieve_byte=(recieve_byte<<1)|MDI;
				MCO=0;
			} //������� ����
			
			buf[read_byte_counter]=recieve_byte;
			read_byte_counter++;


			MDE=1;//�� �������� � �������� ACK
			if(read_byte_counter<len) //ack
			{
					//ACK	
				MDO=0;

			}
			else  //nack
			{
				//NACK		
				MDO=1;			
			}
				
			MCO=1;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=0;
				
			MDE=0;//�����

	  }
	  MDE=1;
	  ERROR_I2C=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
//#pragma OT(0,Speed)
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len))//�������� ����� ������ ������ I2C
 {  
	   static volatile unsigned char write_byte_counter,bit_counter,write_byte;
	  PT_BEGIN(pt);
	  MDE=1;//�������� 
	  write_byte_counter=0;
	  while(write_byte_counter<len)  //���� �������� ������
	  {
	  	//	bit_counter=0;

			write_byte=buf[write_byte_counter];
			write_byte_counter++;
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			for(bit_counter=0;bit_counter<8;bit_counter++)
			{
				
				write_byte=write_byte<<1;//>>1;	 //???
				MDO=CY;
			
				MCO=1;
				PT_YIELD(pt);//����� ������ ��������� �����
				MCO=0;
//				PT_YIELD(pt);//����� ������ ��������� �����

			}
			MDE=0;//����� 
			MCO=0;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=1;
			PT_YIELD(pt);//����� ������ ��������� �����
			MCO=0;	
			MDE=1;//��������
			if(MDI) //NACK
			{
				ERROR_I2C=1;//��������� ������	
				PT_EXIT(pt);
			}
	  }
	  ERROR_I2C=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
//#pragma OT(0,Speed)
static PT_THREAD(I2C_Read_Complete(struct pt *pt))//������������� ������
{
	PT_BEGIN(pt);
   if(i2c_channels.I2C_CHNL.channels.CRC==CRC_Check(i2c_channels.I2C_CHNL.i2c_buf,10))
   {
 	channels[11].channel_data=i2c_channels.I2C_CHNL.channels.DOL;


	switch(i2c_channels.I2C_CHNL.channels.frequency_modific)  //�������� ��� ������ �������� ��������
	{
		case  CHNL_FREQ_256:
		{
				channels[12].channel_data=i2c_channels.I2C_CHNL.channels.frequency;
				channels[12].settings.set.modific=CHNL_FREQ_256;
				channel_number=CHANNEL_NUMBER-1; //������� ����� ���������� ������� �������
				
		}
		break;

		case CHNL_FREQ_COUNT_T:
		{
				channels[12].channel_data=i2c_channels.I2C_CHNL.channels.frequency;
				channels[13].channel_data=i2c_channels.I2C_CHNL.channels.mid_frequency;
				channels[12].settings.set.modific=CHNL_FREQ_COUNT_T;
				channel_number=CHANNEL_NUMBER; //��� ������ �������������

		}
		break;

		default:
		{
		}
		break;
	}

	STATE_BYTE|=i2c_channels.I2C_CHNL.channels.state_byte;//��������� ���� ���������
   }
   i2c_buffer[0]=0;//�� ���������� ���� �������������
	
	PT_END(pt);	
}