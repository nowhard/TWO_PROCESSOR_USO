#include "proto_uso.h"
#include "calibrate\calibrate.h"
#include "i2c.h"
#include <intrins.h>
#include "crc_table.h"
#include "channels.h"
#include "watchdog.h"
#include "modbus\modbus_ascii.h"
//-----------------------------------------------------------------------------------

sbit DE_RE=P3^5;

//-----------------------------------------------------------------------------------
volatile unsigned char xdata DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //��� ����������
volatile unsigned char xdata NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2012 -->";//���������� 	
volatile unsigned char xdata VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// ������ ��������� ���	�� ������ 5 ����

volatile unsigned char xdata ADRESS_DEV=0x1;

volatile unsigned char xdata dev_desc_len=20;//����� �������� ����������
//--------------------------------global variable------------------------------------
volatile unsigned char idata	RECIEVED=0;//�������
volatile unsigned char xdata    recieve_count;//������� ��������� ������
volatile unsigned char xdata	transf_count;//������� ������������ ������	   
volatile unsigned char xdata	buf_len;//����� ������������� ������

//------------------------����� ������--------------------------------
volatile unsigned char idata  CUT_OUT_NULL;//����-�������� 0 ����� 0xD7
volatile unsigned char xdata frame_len=0;//����� �����, ������� ����������� �� ������� ����� �����
//--------------------------------------------------------------------
volatile unsigned char xdata  RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //����� ����������� ������
//volatile unsigned char xdata 			*TransferBuf;
volatile unsigned char xdata  TransferBuf[MAX_LENGTH_TR_BUF]={0} ; //����� ������������ ������
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//���� ��������� ����������
volatile unsigned char idata symbol=0xFF;//�������� ������

volatile unsigned char xdata protocol_type=PROTO_TYPE_NEW;//��� ���������

volatile struct pt pt_proto, pt_buf_handle;

struct RingBuf rngbuf;
//-----------------------------------------------------------------------------------
union //����������� ��� ��������������� char->long
{
	float result_float;
	unsigned char result_char[4];
}
sym_8_to_float;

extern unsigned char idata i2c_buffer[6];
extern unsigned char channel_number;//���������� �������

//-----------------------------------------------------------------------------------
//#pragma OT(0,Speed)
void UART_ISR(void) interrupt 4 //using 1
{	
	EA=0;	//������ ����������
	
	if(RI)
	{
		RI=0; 
		rngbuf.buf[rngbuf.tail]=SBUF;
		rngbuf.tail++;
		rngbuf.count++;										
	}
//----------------------------��������----------------------------------------------------------------
	if(TI)
	{
		TI=0;
		 
		if(transf_count<buf_len)
		{

				SBUF=TransferBuf[transf_count];			
				transf_count++;
		}
		else
		{
			transf_count=0;		//�������� �������
			PT_RESTART_OUT(pt_proto);  //������������� �������			
		}					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
//#pragma OT(6,Speed)
void Protocol_Init(void) //using 0
{
	TI=0;
	RI=0;
	
//	TransferBuf=&RecieveBuf[0];	 //����� ������ =����� �������

	Restore_Dev_Address_Desc();

	rngbuf.tail=0;
	rngbuf.head=0;
	rngbuf.count=0;

	recieve_count=0x0;//������� ������ ������
	transf_count=0x0;//������� ������������ ������
	buf_len=0x0;//����� ������������� ������
	DE_RE=0;//����� �� �����
	CUT_OUT_NULL=0;
	STATE_BYTE=0xC0;
	PT_INIT(&pt_proto);
	PT_INIT(&pt_buf_handle);
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //������� ���������� �� ����������
{
	    unsigned char    i=0;
	   									
	   //��������� �����---
	   TransferBuf[0]=0x00;
	   TransferBuf[1]=0xD7;
	   TransferBuf[2]=0x29;
	   //------------------
	   TransferBuf[3]=ADRESS_DEV;  // ����� ����
	   TransferBuf[4]=GET_DEV_INFO_RESP;  // ��� ��������
	   TransferBuf[6]=STATE_BYTE;

	   for(i=0;i<20;i++)
	   {				  // ���������� ������������ �������
			   if(i<DEVICE_NAME_LENGTH_SYM)
			   {
			     	TransferBuf[i+7]=DEV_NAME[i];
			   }
			   else
			   {
			   		TransferBuf[i+7]=0x00;
			   }
		}
	
	   for(i=0;i<5;i++)                   // ���������� ������ ���
	   {
	       if(i<DEVICE_VER_LENGTH_SYM)
		   {
		    	 TransferBuf[i+27]=VERSION[i];
		   }
	   }

	   TransferBuf[32]=channel_number;		   // ���������� �������

	   for(i=0;i<channel_number;i++)				   // ������ �� ������
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ���� ������
		  	TransferBuf[i*2+33+1]=0x00;							// ������ ����
	   }	
	   for(i=0;i<dev_desc_len;i++)					// ���������� ����������
	   {
			 TransferBuf[i+33+channel_number*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+channel_number*2+dev_desc_len;			// ������� ����� ������ 
	   TransferBuf[33+channel_number*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+channel_number*2+dev_desc_len); // ������� ����������� �����

	return (34+channel_number*2+dev_desc_len);
}
//-----------------------------------------------------------------------------
unsigned char Node_Full_Init(void) //using 0 //������ ������������� ����
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_List_Init(void) //using 0 //������������� ������ ������� ���� (��� ������ ������);
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data(void) //using 0 //������ ������ �� �������, �������� ���������� ���������;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Set_Parameters(void) //using 0 //���������� ��������� �� �������, �������� ���������� ���������;
{
       unsigned char  index=0, store_data=0;//i=0;
	 
	   while(index<RecieveBuf[5]-1)				   // ������ �� �������
	      {
			  	if(RecieveBuf[6+index]<channel_number)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case CHNL_ADC://���
							{
								if((channels[RecieveBuf[6+index]].settings.set.modific!=RecieveBuf[6+index+1])||(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[RecieveBuf[6+index]].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
								{  
									channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									channels[RecieveBuf[6+index]].settings.set.state_byte_2=RecieveBuf[6+index+3];
									channels[RecieveBuf[6+index]].settings.set.modific	   =RecieveBuf[6+index+1]&0xF;
									store_data=1;
									
								}
								index++;
							}
							break;


							default :
							{
								_nop_();
							}			
					}
					index=index+3;
				}
				else
				{
					return Request_Error(FR_UNATTENDED_CHANNEL);
				}
		  }
	   if(store_data)
	   {
	   		Store_Channels_Data();	//�������� ��������� ������� � ����
	   }

	   return Request_Error(FR_SUCCESFUL);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Order_Query(void) //using 0 //������ ������������������ ������;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data_Order(void) //using 0 //������ ������ �� �������, �������� ������������������ ������;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_State(void) //using 0 //���������� ��������� �� �������, �������� ���������� ���������;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Get_Data_Order_M2(void) //using 0 //������ ������ �� �������, �������� ������������������ ������;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Reset_State_Flags(void) //using 0 //	���������/����� ������ ��������� 
{
	STATE_BYTE=0x40;
	i2c_buffer[0]=0x12;//������� ���� ������������� ��������
	ChannelsInit();
	return	Request_Error(FR_SUCCESFUL);//������ ���, �������������
}
//-----------------------------------------------------------------------------
#pragma OT(9,Speed)
unsigned char Channel_All_Get_Data(void) //using 0 //������ ���������� �� ���� ������� ���� (����������� �����);
{
   unsigned char data index=0,i=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // ����� ����
   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  // ��� ��������
   TransferBuf[6]=STATE_BYTE;

   index=7;//����� ���������
 
    for(i=0;i<channel_number;i++)				   // ������ �� �������
    {
		  TransferBuf[index++]=i;
		  TransferBuf[index++]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ��� � ����������� ������
		  switch(channels[i].settings.set.type)
		    {
				 case CHNL_ADC:  //���������� �����
				 {
					 switch(channels[i].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[i].calibrate.cal.calibrate==1)//�������������
								{			 			 
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[1];
								}
								else
								{
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[1];	
								} 

								  
								 TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
		                         TransferBuf[index++]=channels[i].settings.set.state_byte_2;	 // ������ ���� ��������� ������
						  }
						  break; 

						  case CHNL_ADC_FIX_24:
						  {
						        if(channels[i].calibrate.cal.calibrate==1)//�������������
								{			 									  
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[1];
								}
								else
								{									 
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[1];	
								} 
		  
								  TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
		                          TransferBuf[index++]=channels[i].settings.set.state_byte_2;	 // ������ ���� ��������� ������
						  }
						  break;
					  }
				  }
				  break;

			 	case CHNL_DOL:	 //���
				{
					  switch(channels[i].settings.set.modific)
				      {	  
							  case CHNL_DOL_ENC:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[1];
										TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[0];
									    TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
							  }
							  break; 
					   }
				}
				break;

				 case CHNL_FREQ: //���������
				 { 
					  switch(channels[i].settings.set.modific)
				      {	  
							  
							  case CHNL_FREQ_COUNT_T:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
									  TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
							  }
							  break;

							  case CHNL_FREQ_256:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
									    TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
							  }
							  break; 
					   }
				  }
				  break;		 
		  }
	   }

	  TransferBuf[5]=index+2-7; 						 // ������� ����� ������ ������ ��� 1(���� �������)+1(����������� �����)
	  TransferBuf[index]=CRC_Check(&TransferBuf[1],(unsigned int)(index)-1); // ������� ��
	  return (unsigned char)(index+1);
}
//-----------------------------------------------------------------------------
#pragma OT(0,Speed)
unsigned char Channel_Set_Calibrate(void)//���������� ������� ��� ������ ����� ����������
{

	float K,C;	

	sym_8_to_float.result_char[0]=RecieveBuf[11];
	sym_8_to_float.result_char[1]=RecieveBuf[10];
	sym_8_to_float.result_char[2]=RecieveBuf[9];
	sym_8_to_float.result_char[3]=RecieveBuf[8];

   	K=sym_8_to_float.result_float;

 	sym_8_to_float.result_char[0]=RecieveBuf[15];
	sym_8_to_float.result_char[1]=RecieveBuf[14];
	sym_8_to_float.result_char[2]=RecieveBuf[13];
	sym_8_to_float.result_char[3]=RecieveBuf[12];

   	C=sym_8_to_float.result_float;

	switch(RecieveBuf[7])
	{
		case 0:
		{
			Calibrate(RecieveBuf[6],K,C);	
		}
		break;

		case 1:
		{
			Calibrate_Set_Flag(RecieveBuf[6],RESET);
		}
		break;

		case 2:
		{
			Calibrate_Set_Flag(RecieveBuf[6],SET);	
		}
		break;

		default :
		{
			_nop_();
		}
		break;
	}


	return	Request_Error(FR_SUCCESFUL);//������ ���, �������������	
}
//------------------------------------------------------------------------------
unsigned char Channel_Get_Calibrate_Value(void)//�������� ����������� ���������� ��������� ������
{
   unsigned char data channel=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // ����� ����
   TransferBuf[4]=CHANNEL_GET_CALIBRATE_RESP;  // ��� ��������
   TransferBuf[5]=0xB;//����� ���������� ����� �����


   channel=RecieveBuf[6];

   if(channel>=channel_number)
   {
   		return Request_Error(FR_COMMAND_STRUCT_ERROR);//����������� ����� ������	
   }

   TransferBuf[6]=channel;

   TransferBuf[7]=channels[channel].calibrate.cal.calibrate;

   sym_8_to_float.result_float=channels[channel].calibrate.cal.K;

   	TransferBuf[11]=sym_8_to_float.result_char[0];
	TransferBuf[10]=sym_8_to_float.result_char[1];
	TransferBuf[9]=sym_8_to_float.result_char[2];
	TransferBuf[8]=sym_8_to_float.result_char[3];

   sym_8_to_float.result_float=channels[channel].calibrate.cal.C;

   	TransferBuf[15]=sym_8_to_float.result_char[0];
	TransferBuf[14]=sym_8_to_float.result_char[1];
	TransferBuf[13]=sym_8_to_float.result_char[2];
	TransferBuf[12]=sym_8_to_float.result_char[3];

	TransferBuf[16]=CRC_Check(&TransferBuf[1],15); // ������� ��
	return 0x11;//17	
}
//------------------------------------------------------------------------------
unsigned char Channel_Set_Address_Desc(void)//���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������
{
	unsigned char  desc_len=0;
	desc_len=RecieveBuf[5]-27;

	if(desc_len)
	{	
   		Store_Dev_Address_Desc(RecieveBuf[6],&RecieveBuf[7],&RecieveBuf[27],&RecieveBuf[32],desc_len);
		return 	  Request_Error(FR_SUCCESFUL);//������ ���, �������������
	}
	else
	{
		return Request_Error(FR_COMMAND_STRUCT_ERROR);
	}
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_All_Default(void)//���������� ��������� � ���������� ������� �� ���������
{
	Channels_Set_Default();
	Calibrate_Set_Default();
	return	Request_Error(FR_SUCCESFUL);//������ ���, �������������		
}
//-----------------------------------------------------------------------------
unsigned char Request_Error(unsigned char error_code) //using 0 //	��������� ������/�����;
{
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  // ����� ����
    TransferBuf[7]=RecieveBuf[4]; // ��� ������� �������
    TransferBuf[4]=0xFF;  // ��� ��������

	TransferBuf[6]=STATE_BYTE; // ���� ������� ����
    TransferBuf[8]=error_code;	 
    TransferBuf[5]=0x04;	  // ����� ������
    TransferBuf[9]=CRC_Check(TransferBuf,9);
	return 10;
}

//-----------------------------------------------------------------------------
unsigned char Old_CRC_Check(unsigned char xdata *Spool_pr,unsigned char Count_pr)
{
     unsigned char crc = 0x0;
	 unsigned char i=0;

	 for(i=0;i<Count_pr;i++)
	 {
	 	CY=0;
		crc+=Spool_pr[i];
		if(CY)
		{
			crc++;
		}
	 }
	 crc=0xFF-crc;

     return crc;	
}
//-----------------------------------------------------------------------------
unsigned char Old_Channel_Get_Data(void)
{
  unsigned char channel=0;
  unsigned char len=0;
  channel=((RecieveBuf[4]>>3)&0x1F);
  if(channel<CHANNEL_NUMBER)
  {  
	   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x28;
	   TransferBuf[3]=0x0;  // 
	   TransferBuf[4]=RecieveBuf[4];
	   TransferBuf[5]=((OLD_CHANNEL_GET_DATA<<5)&0x1F)|0x3;//0x63;// ��� ��������	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //���������� �����
				 {
					 switch(channels[channel].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[channel].calibrate.cal.calibrate==1)//�������������
								{			 			 
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data_calibrate))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data_calibrate))[2];
								}
								else
								{
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[2];	
								} 					
						  }
						  break; 

						  case CHNL_ADC_FIX_24:
						  {
						        if(channels[channel].calibrate.cal.calibrate==1)//�������������
								{			 									  
									 	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data_calibrate))[1];
			    					  	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data_calibrate))[2];
								}
								else
								{									 
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[2];	
								}

						  }
						  break;
					  }					  
				  }
				  break;

			 	case CHNL_DOL:	 //���
				{
					  switch(channels[channel].settings.set.modific)
				      {	  
							  case CHNL_DOL_ENC:
							  {
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
									 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];
//			    					  	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data))[0];
//										TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data))[1];
							  }
							  break; 
					   }
				}
				break;

				 case CHNL_FREQ: //���������
				 { 
					  switch(channels[channel].settings.set.modific)
				      {	  
							  
							  case CHNL_FREQ_COUNT_T:
							  {
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
									 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];

							  }
							  break;

							  case CHNL_FREQ_256:
							  {
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
									 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];
							  }
							  break; 
					   }					   
				  }
				  break;		 
		  }
	   
	  // len=Proto_Paste_Null(TransferBuf,8);

	   TransferBuf[len]=Old_CRC_Check(TransferBuf,len);
	   return len+1;
   }
   return 0;	
}
//-----------------------------------------------------------------------------
unsigned char Old_Channel_Get_Data_State(void)
{
  unsigned char channel=0;
  unsigned char len=0;
  channel=((RecieveBuf[4]>>3)&0x1F);
  if(channel<CHANNEL_NUMBER)
  {
	   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x28;
	   TransferBuf[3]=0x0;  // 
	   TransferBuf[4]=RecieveBuf[4];
	   TransferBuf[5]=((OLD_CHANNEL_GET_DATA_STATE<<5)&0x1F)|0x6;//0x66;// ��� ��������	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //���������� �����
				 {
					 switch(channels[channel].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[channel].calibrate.cal.calibrate==1)//�������������
								{			 			 
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data_calibrate))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data_calibrate))[2];
								}
								else
								{
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[2];	
								} 					
						  }
						  break; 

						  case CHNL_ADC_FIX_24:
						  {
						        if(channels[channel].calibrate.cal.calibrate==1)//�������������
								{			 									  
									 	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data_calibrate))[1];
			    					  	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data_calibrate))[2];
								}
								else
								{									 
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[1];
			    					  	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[2];	
								}

						  }
						  break;
					  }
					  
				  	  switch(channels[channel].settings.set.state_byte_1&0x7)
					  {
					  		case PROTO_ADC_AMP_1:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // ������ ���� ��������� ������
							}
							break;

					  		case PROTO_ADC_AMP_2:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_2<<1))&0xFE;	 // ������ ���� ��������� ������	
							}
							break;

					  		case PROTO_ADC_AMP_32:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_32<<1))&0xFE;	 // ������ ���� ��������� ������	
							}
							break;

					  		case PROTO_ADC_AMP_128:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_128<<1))&0xFE;	 // ������ ���� ��������� ������
							}
							break;

					  		default :
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // ������ ���� ��������� ������
							}
							break;
					  }
					  
					  TransferBuf[9]=0x0A;	 // ������ ���� ��������� ������

				  }
				  break;

			  case CHNL_DOL:	 //���
			  {
				  switch(channels[channel].settings.set.modific)
			      {	  
						  case CHNL_DOL_ENC:
						  {
							 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
							 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];
//			    					  	TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data))[0];
//										TransferBuf[channel]=((unsigned char*)(&channels[channel].channel_data))[1];

								

								switch(channels[channel].settings.set.state_byte_1&0x7)
								{
								  		case PROTO_ADC_AMP_1:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // ������ ���� ��������� ������
										}
										break;
			
								  		case PROTO_ADC_AMP_2:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_2<<1))&0xFE;	 // ������ ���� ��������� ������	
										}
										break;
			
								  		case PROTO_ADC_AMP_32:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_32<<1))&0xFE;	 // ������ ���� ��������� ������	
										}
										break;
			
								  		case PROTO_ADC_AMP_128:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_128<<1))&0xFE;	 // ������ ���� ��������� ������
										}
										break;
			
								  		default :
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;
										}
										break;			
								}
								TransferBuf[9]=0x00;	 // ������ ���� ��������� ������
						  }
						  break; 
					   }
				}
				break;

				 case CHNL_FREQ: //���������
				 { 
					  switch(channels[channel].settings.set.modific)
				      {	  
							  
							  case CHNL_FREQ_COUNT_T:
							  {
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
									 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];

							  }
							  break;

							  case CHNL_FREQ_256:
							  {
									 	TransferBuf[6]=((unsigned char*)(&channels[channel].channel_data))[2];
									 	TransferBuf[7]=((unsigned char*)(&channels[channel].channel_data))[3];
							  }
							  break; 
					   }
					   
					TransferBuf[8]=0x40;	 // ������ ���� ��������� ������
       				TransferBuf[9]=0x00;	 // ������ ���� ��������� ������
				  }
				  break;		 
		  }

	   TransferBuf[10]=0xFF;

	   //len=Proto_Paste_Null(TransferBuf,11);

	   TransferBuf[len]=Old_CRC_Check(TransferBuf,len);
	   return len+1;
   }
   return 0;

}
//-----------------------------------------------------------------------------
unsigned char Old_Channel_Get_State(void)
{
  unsigned char channel=0;
  unsigned char len=0;
  channel=((RecieveBuf[4]>>3)&0x1F);
  if(channel<CHANNEL_NUMBER)
  {
  
	   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x28;
	   TransferBuf[3]=0x0;  // 
	   TransferBuf[4]=RecieveBuf[4];
	   TransferBuf[5]=((OLD_CHANNEL_GET_STATE<<5)&0x1F)|0x3;//0x43;// ��� ��������	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //���������� �����
				 {		  
					  switch(channels[channel].settings.set.state_byte_1&0x7)
					  {
					  		case PROTO_ADC_AMP_1:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_1<<1);	 // ������ ���� ��������� ������
							}
							break;

					  		case PROTO_ADC_AMP_2:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_2<<1);	 // ������ ���� ��������� ������	
							}
							break;

					  		case PROTO_ADC_AMP_32:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_32<<1);	 // ������ ���� ��������� ������	
							}
							break;

					  		case PROTO_ADC_AMP_128:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_128<<1);	 // ������ ���� ��������� ������
							}
							break;

					  		default :
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_1<<1);	 // ������ ���� ��������� ������
							}
							break;
					  }
					  
					  TransferBuf[7]=0x0A;	 // ������ ���� ��������� ������
				  }
				  break;

			 	case CHNL_DOL:	 //���
				{

						switch(channels[channel].settings.set.state_byte_1&0x7)
						{
						  		case PROTO_ADC_AMP_1:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_1<<1);	 // ������ ���� ��������� ������
								}
								break;
	
						  		case PROTO_ADC_AMP_2:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_2<<1);	 // ������ ���� ��������� ������	
								}
								break;
	
						  		case PROTO_ADC_AMP_32:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_32<<1);	 // ������ ���� ��������� ������	
								}
								break;
	
						  		case PROTO_ADC_AMP_128:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_128<<1);	 // ������ ���� ��������� ������
								}
								break;
	
						  		default :
								{
									TransferBuf[6]=0x40;	 // ������ ���� ��������� ������
								}
								break;
	
						}
						TransferBuf[7]=0x00;	 // ������ ���� ��������� ������
				}
				break;

				 case CHNL_FREQ: //���������
				 { 
					   
					TransferBuf[6]=0x40;	 // ������ ���� ��������� ������
       				TransferBuf[7]=0x00;	 // ������ ���� ��������� ������
				 }
				 break;		 
		  }
	   
	  // len=Proto_Paste_Null(TransferBuf,8);

	   TransferBuf[len]=Old_CRC_Check(TransferBuf,len);
	   return len+1;
   }
   return 0;	
}
//-----------------------------------------------------------------------------
unsigned char Old_Reinit_Block(void)
{
	return 0;	
}
//-----------------------------------------------------------------------------
unsigned char Old_Channel_Set_ADC_Range(void)
{
  unsigned char channel=0;
  channel=((RecieveBuf[4]>>3)&0x1F);
  if(channel<CHANNEL_NUMBER)
  {	
	switch((RecieveBuf[6]>>1)&0x3)
	{
	  		case OLD_PROTO_ADC_AMP_1:
			{
			     channels[channel].settings.set.state_byte_1&=0xF0;
				 channels[channel].settings.set.state_byte_1|=PROTO_ADC_AMP_1;
			}
			break;

	  		case OLD_PROTO_ADC_AMP_2:
			{
				 channels[channel].settings.set.state_byte_1&=0xF0;
				 channels[channel].settings.set.state_byte_1|=PROTO_ADC_AMP_2;
			}
			break;

	  		case OLD_PROTO_ADC_AMP_32:
			{
				 channels[channel].settings.set.state_byte_1&=0xF0;
				 channels[channel].settings.set.state_byte_1|=PROTO_ADC_AMP_32;
			}
			break;

	  		case OLD_PROTO_ADC_AMP_128:
			{
			     channels[channel].settings.set.state_byte_1&=0xF0;
				 channels[channel].settings.set.state_byte_1|=PROTO_ADC_AMP_128;
			}
			break;

	  		default :
			{
			     channels[channel].settings.set.state_byte_1&=0xF0;
				 channels[channel].settings.set.state_byte_1|=PROTO_ADC_AMP_1;
			}
			break;
	}
  }
  return 0;
}
//-----------------------------------------------------------------------------
unsigned char Proto_Paste_Null(unsigned char *buf,unsigned char len)//�.�. ������ �������� ���������� � ����� ���������� 0 ����� D7 �� ������������ ��� ����������
{
	volatile unsigned char xdata i=0,j=0;

	for(i=3;i<len;i++) //��������� ��������� 0x00 0xD7 0x28
	{
		 if(buf[i]==0xD7)
		 {
		 	 for(j=(len-1);j>i;j--)
			 {
			 	buf[j+1]=buf[j];//�������� �������� ������ ������ �� ���� ������� �� D7
			 }
			 buf[i+1]=0x0;//��������� 0x00 ����� 0xD7

			 if(protocol_type==PROTO_TYPE_OLD)
			 {
			 	buf[5]++;//����� ����� ����������� �� 1
			 }

			 len++;
			 i++;//�� ��������� 0 �������
		 }
	}
	return len;
}
//-----------------------------------------------------------------------------
void ProtoBufHandling(void) //using 0 //������� ��������� ��������� �������
{
	switch(protocol_type)
	{
		case PROTO_TYPE_NEW:
		{
			  channels[11].settings.set.state_byte_1=0x40; //����������� ����� ������� ����
			  channels[11].settings.set.state_byte_2=0x0A;
			  switch(RecieveBuf[4])
			  {
			//---------------------------------------
			  	case GET_DEV_INFO_REQ:
				{
					buf_len=Send_Info();	
				}
				break;
			//---------------------------------------
			  	case NODE_FULL_INIT_REQ:
				{
					buf_len=Node_Full_Init();
				}
				break;
			//---------------------------------------
			  	case CHANNEL_LIST_INIT_REQ:
				{	
					buf_len=Channel_List_Init();	
				}
				break;
			//---------------------------------------
				case CHANNEL_GET_DATA_REQ:
				{
					buf_len=Channel_Get_Data();	
				}
				break;
				//-----------------------------------
				case CHANNEL_SET_PARAMETERS_REQ:
				{
					buf_len=Channel_Set_Parameters();
				}
				break;
				//-----------------------------------
				case CHANNEL_SET_ORDER_QUERY_REQ:
				{
					buf_len=Channel_Set_Order_Query();
				}
				break;
			//----------------------------------------
				case CHANNEL_GET_DATA_ORDER_REQ:
				{
					 buf_len=Channel_Get_Data_Order();
				}
				break;
			//----------------------------------------
				case CHANNEL_SET_STATE_REQ:
				{
					 buf_len=Channel_Set_State();
				}
				break;
			//----------------------------------------
				case CHANNEL_GET_DATA_ORDER_M2_REQ:
				{
					 buf_len=Channel_Get_Data_Order_M2();
				}
				break;
			//------------------------------------------
				case CHANNEL_SET_RESET_STATE_FLAGS_REQ:
				{
					buf_len=Channel_Set_Reset_State_Flags();
				}
				break;
			//------------------------------------------
				case CHANNEL_ALL_GET_DATA_REQ:
				{
					 buf_len=Channel_All_Get_Data();
				}
				break;
			//------------------------------------------
				case CHANNEL_SET_ADDRESS_DESC:
				{
					 buf_len=Channel_Set_Address_Desc();
				}
				break;
			//------------------------------------------
				case CHANNEL_SET_CALIBRATE:
				{
					 buf_len=Channel_Set_Calibrate();
				}
				break;
			//------------------------------------------
				case CHANNEL_SET_ALL_DEFAULT:
				{
					 buf_len=Channel_Set_All_Default();
				}
				break;
			//------------------------------------------
				case CHANNEL_GET_CALIBRATE_REQ:
				{
					 buf_len=Channel_Get_Calibrate_Value();
				}
				break;
			//------------------------------------------
			    default:
				{
				   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
			    }								   
			  }
		}
		break;

		case PROTO_TYPE_OLD: //������ ��������
		{
			  switch((RecieveBuf[5]>>5)&0x7)
			  {
			//---------------------------------------
			  	case OLD_CHANNEL_REINIT_BLOCK:
				{
					buf_len=Old_Reinit_Block();	
				}
				break;
			//---------------------------------------
			  	case OLD_CHANNEL_SET_ADC_RANGE:
				{
					buf_len=Old_Channel_Set_ADC_Range();
				}
				break;
			//---------------------------------------
			  	case OLD_CHANNEL_GET_STATE:
				{	
					buf_len=Old_Channel_Get_State();	
				}
				break;
			//---------------------------------------
				case OLD_CHANNEL_GET_DATA:
				{
					buf_len=Old_Channel_Get_Data();	
				}
				break;
				//-----------------------------------
				case OLD_CHANNEL_GET_DATA_STATE:
				{
					buf_len=Old_Channel_Get_Data_State();
				}
				break;
				//-----------------------------------
			    default:
				{
				   
			    }								   
			  }	
		 }
		 break;
		 
		 case PROTO_TYPE_MODBUS_ASCII:
		 {
			  switch(MBCHAR2BIN(RecieveBuf[3])<<4| MBCHAR2BIN(RecieveBuf[4]))
			  {
				//---------------------------------------
				  	case MB_FUNC_READ_HOLDING_REGISTER:
					{
						buf_len=ReadHoldingReg();	
					}
					break;
				//------------------------------------------
				    default:
					{
					   buf_len=0x0;
				    }								   
			  }
		 }
		 break;	
	}

  return;
}
//-----------------------------------------------------------------------------------
PT_THREAD(ProtoProcess(struct pt *pt))
 {
 static unsigned char  CRC=0x0, LRC=0x0;
  PT_BEGIN(pt);

  while(1) 
  {
  //----------restart------------
		recieve_count=0x0;//??
		REN=1;//recieve enqble
		DE_RE=0;//����� �� �����
		ES=1;
  //-----------------------------
	  // PT_WAIT_UNTIL(pt,RECIEVED); //���� ������� �� �����
	   wdt_count[Proto_Proc].process_state=IDLE;

	   PT_YIELD_UNTIL(pt,RECIEVED); //���� ������� �� �����	
	   wdt_count[Proto_Proc].count++;

	    RECIEVED=0;
		

	 	switch(protocol_type)
		{
			case PROTO_TYPE_NEW:
			{
				if(RecieveBuf[3]!=ADRESS_DEV)//���� ����� ������	  
				{
					PT_RESTART(pt);//���� ����� �� �������-������������ ��������			
				}

				CRC=RecieveBuf[recieve_count-1];

			    if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
				{
					PT_RESTART(pt);//���� CRC �� �������-������������ ��������	 
				}
			}
			break;
	
			case PROTO_TYPE_OLD:
			{
				CRC=RecieveBuf[recieve_count-1];

				if(Old_CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
				{
					PT_RESTART(pt);//���� CRC �� �������-������������ ��������	 
				}
			}
			break;
	
			case PROTO_TYPE_MODBUS_ASCII:
			{
				if((unsigned char)(MBCHAR2BIN(RecieveBuf[1])<<4| MBCHAR2BIN(RecieveBuf[2]))!=ADRESS_DEV)
				{
					PT_RESTART(pt);	
				}	
				
	   	
				LRC=((MBCHAR2BIN(RecieveBuf[recieve_count-LRC_LEN])<<4)|(MBCHAR2BIN(RecieveBuf[recieve_count-LRC_LEN+1])));
					
				if(LRC_Check(&RecieveBuf,(recieve_count-LRC_LEN))!=LRC)
				{
					PT_RESTART(pt);//���� LRC �� �������-������������ ��������	 		 
				}
			}
			break;
		}

	//	PT_YIELD(pt);//����� ������ ��������� �����
  //-----------------------------
  		ProtoBufHandling();//��������� ��������� ���������	
		if(buf_len==0)//���� � ������ �����
		{
			PT_RESTART(pt);//������������ ��������	
		}
		else
		{
			buf_len=Proto_Paste_Null(TransferBuf,buf_len);
			DE_RE=1; //����������� RS485 � ����� ��������
							
			REN=0;	//������ ������-������ ��������
			transf_count=0;
		//	CUT_OUT_NULL=0;
			SBUF=TransferBuf[transf_count];//�������� �����, ��������� �������� �������
			transf_count++;//�������������� ������� ����������
			ES=1; //������� ���������� ����	

		    if(protocol_type==PROTO_TYPE_NEW)
			{
				PT_DELAY(pt,10);
			}
			else
			{
				PT_DELAY(pt,3);
			}			
		}		
  //-----------------------------
  }

 PT_END(pt);
}
//-----------------------------------------------------------------------------------------------
void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len)//��������� � ���� ����� ����� ����������, ���, ������, ��������
{
//������������
	
	unsigned int blocks=0;//����� ����� ���������� ������, ������ �� ���������� �����������
	
	EEPROM_Write(&addr,1,DEVICE_ADDR_EEPROM);	 //1 ����
	EEPROM_Write(name,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 ������
	memcpy(DEV_NAME,name,DEVICE_NAME_LENGTH_SYM);//�������� ���������� ��� � �����

	EEPROM_Write(ver,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);		//2 �����
	memcpy(VERSION,ver,DEVICE_VER_LENGTH_SYM);//�������� ������ � �����

	blocks=desc_len>>2;//� ����� 4 �����, ����� �� 4
	if(desc_len&0xFC)//���� ���� �������, �� ����� ��� ���� ����
		blocks++;
	
	if(blocks>DEVICE_DESC_MAX_LENGTH) //����������� �� ���������� ������ ��� �����������
		blocks=DEVICE_DESC_MAX_LENGTH;

	EEPROM_Write(desc,(unsigned int)blocks,DEVICE_DESC_EEPROM);
	dev_desc_len=desc_len;
	EEPROM_Write(&desc_len,1,DEVICE_DESC_LEN_EEPROM);//�������� ����� �����������
	
	memcpy(NOTICE,desc,desc_len);//�������� �������� � �����
	
	return;
}
//-----------------------------------------------------------------------------------------------
void Restore_Dev_Address_Desc(void)//������������ �� ���� ����� � ���������� �� ����������
{
	unsigned int blocks=0;//����� ����� ���������� ������, ������ �� ���������� �����������
	
	EEPROM_Read(&ADRESS_DEV,1,DEVICE_ADDR_EEPROM);	 //1 ���� ��������������� ����� �� ����

	if(ADRESS_DEV<1 || ADRESS_DEV>15)
		ADRESS_DEV=1; 

	EEPROM_Read(DEV_NAME,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 ������
    EEPROM_Read(VERSION,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);

	EEPROM_Read(&dev_desc_len,1,DEVICE_DESC_LEN_EEPROM);//����� �������� ���������� � ������

	if(dev_desc_len>DEVICE_DESC_MAX_LENGTH_SYM)
		dev_desc_len=DEVICE_DESC_MAX_LENGTH_SYM;

	blocks=dev_desc_len>>2;//� ����� 4 �����, ����� �� 4
	if(dev_desc_len&0xFC)//���� ���� �������, �� ����� ��� ���� ����
		blocks++;

	if(blocks>DEVICE_DESC_MAX_LENGTH) //����������� �� ���������� ������ ��� �����������	 ???
		blocks=DEVICE_DESC_MAX_LENGTH;

	EEPROM_Read(NOTICE,(unsigned int)blocks,DEVICE_DESC_EEPROM);
	return;
}
//-----------------------------------------------------------------------------------------------


 PT_THREAD(RingBufHandling(struct pt *pt))//��������� ���������� ������
 {
 	static unsigned char temp_count=0;//��������� ����� �����  � �������� ������
	static unsigned char temp_tail=0;
//	static unsigned char frame_count=0;//������� ������������ �����	 ���������

	static unsigned char i=0;

  PT_BEGIN(pt);

  while(1) 
  {
  		PT_YIELD_UNTIL(pt,(rngbuf.count>0x0));//� ������ ���� ������

		ES=0;
		temp_count=rngbuf.count;
		rngbuf.count=0x0;
		temp_tail =rngbuf.tail;
		ES=1;

		for(i=(temp_tail-temp_count);i!=temp_tail;i++)//����� ��������� ������������������
		{
			switch(rngbuf.buf[i])
			{
					case (char)(0xD7):
					{
						RecieveBuf[recieve_count]=rngbuf.buf[i];
						recieve_count++;
						CUT_OUT_NULL=1;		 
					}
					break;
		
					case (char)(0x29):
					{
						if(CUT_OUT_NULL==1)
						{
							RecieveBuf[0]=0x0;
							RecieveBuf[1]=0xD7;
							RecieveBuf[2]=0x29;
							recieve_count=0x3;
							protocol_type=PROTO_TYPE_NEW;		 	
						}
						else
						{
							RecieveBuf[recieve_count]=rngbuf.buf[i];
							recieve_count++;	
						}
						CUT_OUT_NULL=0;
					}
					break;
		
					case (char)(0x28):
					{
						if(CUT_OUT_NULL==1)
						{
							RecieveBuf[0]=0x0;
							RecieveBuf[1]=0xD7;
							RecieveBuf[2]=0x28;
							recieve_count=0x3;
							protocol_type=PROTO_TYPE_OLD;		 	
						}
						else
						{
							RecieveBuf[recieve_count]=rngbuf.buf[i];
							recieve_count++;	
						}
						CUT_OUT_NULL=0;
					}
					break;
		
					case (char)(0x0):
					{
		 				if(CUT_OUT_NULL==1)	  //���� ����� 0xD7-����������
						{
							CUT_OUT_NULL=0;		
						}
						else
						{
							RecieveBuf[recieve_count]=rngbuf.buf[i];
							recieve_count++;	
						}
					}
					break;
		
					//---------MD ASCII------
//					case  0x3A:	 //":"
//					{
//						RecieveBuf[recieve_count]=symbol;
//						recieve_count++;
//						protocol_type=PROTO_TYPE_MODBUS_ASCII;								 
//					}
//					break;
//		
//					case 0xA:	 //"LF"
//					{
//					  	ES=0;
//					  	REN=0;  //recieve disable 
//					}
//					break;
//		
//					case 0xD:	 //"CR"
//					{
//						//
//					}
//					break;
					//-----------------------
		
					default :
					{
						RecieveBuf[recieve_count]=rngbuf.buf[i];
						recieve_count++;
						CUT_OUT_NULL=0;		
					}
				}
		
			   if(recieve_count>6)
			   {
			   		  if(recieve_count==6+frame_len)	  // ��������� ��������� � frame_len ����� ���� ������, 6 ������ ��� ����� ���� � ������, ���� �������� 5 ����� ������������ � ������������
		   			  {
							RECIEVED=1;//����� ������
					  		ES=0;
						//	rngbuf.tail=0;
					  		REN=0;  //recieve disable -��������� ��������� � �����	
						   	CUT_OUT_NULL=0;  			  			
					  }	 
					  
					  //----------------------������������ ��������� ������ ����� �����-------------
				      if(recieve_count>MAX_LENGTH_REC_BUF)	//���� ������� ������� �������
					  {
							PT_RESTART(&pt_proto);  //������������� �������
					  }  
			   }
			   else
			   {
					   if(recieve_count==6)
					   {     
				        	
							frame_len=RecieveBuf[recieve_count-1];  // �������� ����� ������ ����� ���������
							if(protocol_type==PROTO_TYPE_OLD)
							{
								frame_len&=0x1F;//� ������ ��������� ������ 5 ������� ��� -����� ���������� �����	
							}					 
					   }	   		
			   }														
		}	
  }

  PT_END(pt);
 }