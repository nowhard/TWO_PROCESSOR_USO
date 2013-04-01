#include "proto_uso.h"
#include "calibrate\calibrate.h"
#include "i2c.h"
#include <intrins.h>
//-----------------------------------------------------------------------------------
  unsigned char code Crc8Table[256]={0x00, 0xBC, 0x01, 0xBD, 0x02, 0xBE, 0x03, 0xBF, 
 									 0x04, 0xB8, 0x05, 0xB9, 0x06, 0xBA, 0x07, 0xBB, 
									 0x08, 0xB4, 0x09, 0xB5, 0x0A, 0xB6, 0x0B, 0xB7, 
									 0x0C, 0xB0, 0x0D, 0xB1, 0x0E, 0xB2, 0x0F, 0xB3, 
									 0x10, 0xAC, 0x11, 0xAD, 0x12, 0xAE, 0x13, 0xAF, 
									 0x14, 0xA8, 0x15, 0xA9, 0x16, 0xAA, 0x17, 0xAB, 
									 0x18, 0xA4, 0x19, 0xA5, 0x1A, 0xA6, 0x1B, 0xA7, 
									 0x1C, 0xA0, 0x1D, 0xA1, 0x1E, 0xA2, 0x1F, 0xA3, 
									 0x20, 0x9C, 0x21, 0x9D, 0x22, 0x9E, 0x23, 0x9F, 
									 0x24, 0x98, 0x25, 0x99, 0x26, 0x9A, 0x27, 0x9B, 
									 0x28, 0x94, 0x29, 0x95, 0x2A, 0x96, 0x2B, 0x97, 
									 0x2C, 0x90, 0x2D, 0x91, 0x2E, 0x92, 0x2F, 0x93, 
									 0x30, 0x8C, 0x31, 0x8D, 0x32, 0x8E, 0x33, 0x8F, 
									 0x34, 0x88, 0x35, 0x89, 0x36, 0x8A, 0x37, 0x8B, 
									 0x38, 0x84, 0x39, 0x85, 0x3A, 0x86, 0x3B, 0x87, 
									 0x3C, 0x80, 0x3D, 0x81, 0x3E, 0x82, 0x3F, 0x83, 
									 0x40, 0xFC, 0x41, 0xFD, 0x42, 0xFE, 0x43, 0xFF, 
									 0x44, 0xF8, 0x45, 0xF9, 0x46, 0xFA, 0x47, 0xFB, 
									 0x48, 0xF4, 0x49, 0xF5, 0x4A, 0xF6, 0x4B, 0xF7, 
									 0x4C, 0xF0, 0x4D, 0xF1, 0x4E, 0xF2, 0x4F, 0xF3, 
									 0x50, 0xEC, 0x51, 0xED, 0x52, 0xEE, 0x53, 0xEF, 
									 0x54, 0xE8, 0x55, 0xE9, 0x56, 0xEA, 0x57, 0xEB, 
									 0x58, 0xE4, 0x59, 0xE5, 0x5A, 0xE6, 0x5B, 0xE7, 
									 0x5C, 0xE0, 0x5D, 0xE1, 0x5E, 0xE2, 0x5F, 0xE3, 
									 0x60, 0xDC, 0x61, 0xDD, 0x62, 0xDE, 0x63, 0xDF, 
									 0x64, 0xD8, 0x65, 0xD9, 0x66, 0xDA, 0x67, 0xDB, 
									 0x68, 0xD4, 0x69, 0xD5, 0x6A, 0xD6, 0x6B, 0xD7, 
									 0x6C, 0xD0, 0x6D, 0xD1, 0x6E, 0xD2, 0x6F, 0xD3, 
									 0x70, 0xCC, 0x71, 0xCD, 0x72, 0xCE, 0x73, 0xCF, 
									 0x74, 0xC8, 0x75, 0xC9, 0x76, 0xCA, 0x77, 0xCB, 
									 0x78, 0xC4, 0x79, 0xC5, 0x7A, 0xC6, 0x7B, 0xC7, 
									 0x7C, 0xC0, 0x7D, 0xC1, 0x7E, 0xC2, 0x7F, 0xC3};
sbit DE_RE=P3^5;

//-----------------------------------------------------------------------------------
volatile unsigned char xdata DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //��� ����������
volatile unsigned char xdata NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2012 -->";//���������� 	
volatile unsigned char xdata VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// ������ ��������� ���	�� ������ 5 ����

volatile unsigned char xdata ADRESS_DEV=0x1;

volatile unsigned char xdata dev_desc_len=20;//����� �������� ����������
//--------------------------------global variable------------------------------------
//volatile unsigned char xdata	PROTO_STATE;//������� ���������
volatile unsigned char idata	RECIEVED=0;//�������
volatile unsigned char xdata    recieve_count;//������� ��������� ������
volatile unsigned char xdata	transf_count;//������� ������������ ������	   
volatile unsigned char xdata	buf_len;//����� ������������� ������

volatile unsigned char idata  CUT_OUT_NULL;//����-�������� 0 ����� 0xD7
volatile unsigned char xdata frame_len=0;//����� �����, ������� ����������� �� ������� ����� �����
//--------------------------------------------------------------------
volatile unsigned char xdata  RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //����� ����������� ������
volatile unsigned char xdata 			*TransferBuf;
//static unsigned char /*data*/ volatile  TransferBuf[MAX_LENGTH_TR_BUF] ; //����� ������������ ������
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//���� ��������� ����������
volatile unsigned char idata symbol=0xFF;//�������� ������

volatile unsigned char xdata avaible_channels=0;

volatile struct pt pt_proto;
//-----------------------------------------------------------------------------------
union //����������� ��� ��������������� char->long
{
	float result_float;
	unsigned char result_char[4];
}
sym_8_to_float;
extern unsigned char idata i2c_buffer[6];
//-----------------------------------------------------------------------------------
#pragma OT(0,Speed)
void UART_ISR(void) interrupt 4 //using 1
{	
	EA=0;	//������ ����������
	
	if(RI)
	{
		RI=0; 
//----------------------������������ ��������� ������ ����� �����-------------
		if(recieve_count>MAX_LENGTH_REC_BUF)	//���� ������� ������� �������
		{
			PT_RESTART_OUT(pt_proto);  //������������� �������
			return;
		} 


		symbol=SBUF;
		switch(symbol)
		{
			case (char)(0xD7):
			{
				RecieveBuf[recieve_count]=symbol;
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
				}
				else
				{
					RecieveBuf[recieve_count]=symbol;
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
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;	
				}
			}
			break;

			default :
			{
				RecieveBuf[recieve_count]=symbol;
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
			  		REN=0;  //recieve disable -��������� ��������� � �����	
				   	CUT_OUT_NULL=0;  			  			
			  }	  
	   }
	   else
	   {
			   if(recieve_count==6)
			   {     
		        	frame_len=RecieveBuf[recieve_count-1];  // �������� ����� ������ ����� ���������					 
			   }	   		
	   }										
	}
//----------------------------��������----------------------------------------------------------------
	if(TI)
	{
		TI=0;
		 
		if(transf_count<buf_len)
		{
			if(transf_count<3)//�������� ���������
			{
				SBUF=TransferBuf[transf_count];			
				transf_count++;
			}
			else   //����...   ����������� 0 ����� 0xD7
			{
					if(CUT_OUT_NULL==0)
					{
						if(TransferBuf[transf_count]==(unsigned char)0xD7)//��������, ���  ,0xD7 ��� ������
						{			
							CUT_OUT_NULL=0x1;	
						}
						SBUF=TransferBuf[transf_count];			
						transf_count++;
					}
					else
					{
						SBUF=(unsigned char)0x0;
						CUT_OUT_NULL=0;		
					}	
			}	
		}
		else
		{
			transf_count=0;		//�������� �������
			CUT_OUT_NULL=0;
			PT_RESTART_OUT(pt_proto);  //������������� �������			
		}					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
#pragma OT(6,Speed)
void Protocol_Init(void) //using 0
{
	unsigned char i=0;	

	TI=0;
	RI=0;
	
	TransferBuf=&RecieveBuf[0];	 //����� ������ =����� �������

	Restore_Dev_Address_Desc();


	recieve_count=0x0;//������� ������ ������
	transf_count=0x0;//������� ������������ ������
	buf_len=0x0;//����� ������������� ������
	DE_RE=0;//����� �� �����
	CUT_OUT_NULL=0;
	STATE_BYTE=0xC0;
//-----------------------------//�������� ��������� �������
   for(i=0;i<CHANNEL_NUMBER;i++)
   {
   		if(channels[i].number!=0xFF)
		{
			avaible_channels++;	
		}		
   }
//-----------------------------

	PT_INIT(&pt_proto);
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //������� ���������� �� ����������
{
	    volatile unsigned char    i=0;
	   									
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

	   TransferBuf[32]=avaible_channels;		   // ���������� �������

	   for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� ������
       {
		  	if(channels[i].number!=0xFF)
			{
				TransferBuf[channels[i].number*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ���� ������
			  	TransferBuf[channels[i].number*2+33+1]=0x00;	
			}						// ������ ����
	   }
	   	
	   for(i=0;i<dev_desc_len;i++)					// ���������� ����������
	   {
			 TransferBuf[i+33+avaible_channels*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+avaible_channels*2+dev_desc_len;			// ������� ����� ������ 
	   TransferBuf[33+avaible_channels*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+avaible_channels*2+dev_desc_len); // ������� ����������� �����

	return (34+avaible_channels*2+dev_desc_len);
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
       unsigned char xdata index=0, store_data=0,i=0, chn_index=0;
	 
	   while(index<RecieveBuf[5]-1)				   // ������ �� �������
	      {
			  	if(RecieveBuf[6+index]<avaible_channels)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case CHNL_ADC://���
							{
								
								for(i=0;i<CHANNEL_NUMBER;i++)	//����� ���������������� ������
								{
									if(channels[i].number==RecieveBuf[6+index])	
									{
										chn_index=i;
										break;
									}
								}
								
								if((channels[chn_index].settings.set.modific!=RecieveBuf[6+index+1])||(channels[chn_index].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[chn_index].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
								{  
									channels[chn_index].settings.set.state_byte_1=RecieveBuf[6+index+2];
									channels[chn_index].settings.set.state_byte_2=RecieveBuf[6+index+3];
									channels[chn_index].settings.set.modific	 =RecieveBuf[6+index+1]&0xF;
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
	return	Request_Error(FR_SUCCESFUL);//������ ���, �������������
}
//-----------------------------------------------------------------------------

unsigned char Channel_All_Get_Data(void) //using 0 //������ ���������� �� ���� ������� ���� (����������� �����);
{
   unsigned char data index=0,i=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // ����� ����
   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  // ��� ��������
   TransferBuf[6]=STATE_BYTE;

   index=7;//����� ���������
 
    for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� �������
    {

		if(channels[i].number==0xFF)  //���� ����� ��������, ���������
		{
			continue;
		}		  

		  TransferBuf[index++]=channels[i].number;
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
			channels[RecieveBuf[6]].calibrate.cal.calibrate=0;//���������/������ ���� ����������
			EEPROM_Write(&channels[RecieveBuf[6]].calibrate.serialize,3,ADC_CALIBRATE_ADDR+RecieveBuf[6]*3);
		}
		break;

		case 2:
		{
			channels[RecieveBuf[6]].calibrate.cal.calibrate=1;//���������/������ ���� ����������
			EEPROM_Write(&channels[RecieveBuf[6]].calibrate.serialize,3,ADC_CALIBRATE_ADDR+RecieveBuf[6]*3);	
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
void ProtoBufHandling(void) //using 0 //������� ��������� ��������� �������
{
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
    default:
	{
//       COMMAND_ERR=0x1;//�������������� �������
	   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
//	   PROTO_STATE=PROTO_ERR_HANDLING;//�� ���������� ������
    }								   
  }

  return;
}
//-----------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
#pragma OT(0,Speed) 
PT_THREAD(ProtoProcess(struct pt *pt))
 {
 //unsigned char i=0;
 static unsigned char  CRC=0x0;
  PT_BEGIN(pt);

  while(1) 
  {
  //----------restart------------
		recieve_count=0x0;//??
		REN=1;//recieve enqble
		DE_RE=0;//����� �� �����
		ES=1;
  //-----------------------------
	   PT_WAIT_UNTIL(pt,RECIEVED); //���� ������� �� �����
	   RECIEVED=0;
		
		if(RecieveBuf[3]!=ADRESS_DEV)//���� ����� ������	  
		{
			PT_RESTART(pt);//���� ����� �� �������-������������ ��������			
		}	
				
	    CRC=RecieveBuf[recieve_count-1];
				
		if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
		{
			PT_RESTART(pt);//���� CRC �� �������-������������ ��������	 
		}
		PT_YIELD(pt);//����� ������ ��������� �����
  //-----------------------------
  		ProtoBufHandling();//��������� ��������� ���������	
		if(buf_len==0)//���� � ������ �����
		{
			PT_RESTART(pt);//������������ ��������	
		}
		else
		{
			DE_RE=1; //����������� RS485 � ����� ��������
							
			REN=0;	//������ ������-������ ��������
			transf_count=0;
			CUT_OUT_NULL=0;
			SBUF=TransferBuf[transf_count];//�������� �����, ��������� �������� �������
			transf_count++;//�������������� ������� ����������
			ES=1; //������� ���������� ����	

			PT_DELAY(pt,10);			
		}
  //-----------------------------
  }

 PT_END(pt);
}
//-----------------------CRC------------------------------------------------------------
#pragma OT(6,Speed)
  unsigned char CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr ) 
 {
     unsigned char crc = 0x0;

     while (Count_pr--)
         crc = Crc8Table[crc ^ *Spool_pr++];

     return crc;
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
