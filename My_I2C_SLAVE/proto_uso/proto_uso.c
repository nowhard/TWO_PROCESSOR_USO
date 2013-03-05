#include "proto_uso.h"

sbit DE_RE=P3^5;
 sbit LED=P3^7;
//-----------------------------------------------------------------------------------
volatile unsigned char xdata DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //��� ����������
volatile unsigned char xdata NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2011 -->";//���������� 	
volatile unsigned char xdata VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// ������ ��������� ���	�� ������ 5 ����

volatile unsigned char xdata ADRESS_DEV=0x1;

volatile unsigned char xdata dev_desc_len=20;//����� �������� ����������
//--------------------------------global variable------------------------------------
volatile unsigned char xdata	PROTO_STATE;//������� ���������
volatile unsigned char xdata    recieve_count;//������� ��������� ������
volatile unsigned char xdata	transf_count;//������� ������������ ������	   
volatile unsigned char xdata	buf_len;//����� ������������� ������

//------------------------����� ������--------------------------------
volatile unsigned char xdata CRC_ERR;	//������ �rc
volatile unsigned char xdata COMMAND_ERR;//���������������� �������

volatile unsigned char xdata TIMEOUT;//������� 

volatile unsigned char idata  CUT_OUT_NULL;//����-�������� 0 ����� 0xD7
volatile unsigned char xdata frame_len=0;//����� �����, ������� ����������� �� ������� ����� �����
//--------------------------------------------------------------------
volatile unsigned char xdata  RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //����� ����������� ������
volatile unsigned char xdata 			*TransferBuf;
//static unsigned char /*data*/ volatile  TransferBuf[MAX_LENGTH_TR_BUF] ; //����� ������������ ������
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//���� ��������� ����������
//-----------------------------------------------------------------------------------

void UART_ISR(void) interrupt 4 //using 1
{	
  unsigned char idata symbol=0xFF;//�������� ������

	EA=0;	//������ ����������
	
	if(RI)
	{
		RI=0; 
//----------------------������������ ��������� ������ ����� �����-------------
		if(recieve_count>MAX_LENGTH_REC_BUF)	//���� ������� ������� �������
		{
			PROTO_STATE=PROTO_RESTART;//������ ������� ���������
			return;
		} 
		//TIC_Start(); //���������� ������� ��������

		//------tic start-------
		//TIMECON&=0x0;
	  	//INTVAL=0x1;
	  //	TIMECON=0x1B;//00011011 -���� ������, ���������� 1 �������	
		//-------------


		symbol=SBUF;
		
//--------------------------������ �����...�������� �� ����� �����--------
	    if(recieve_count<6)
		{
				switch(recieve_count)	 
				{
					case  0:   //������ ������ 0
					{
	 				 	 if(symbol!=0x00)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;								 	
						 }	 
					}
					break;
	
					case 1:	 //������ ������ 0xD7
					{
						 if(symbol!=0xD7)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;
						 }		 
					}
					break;
	
					case 2:	 //	������ ������ 0x29
					{
					 	 if(symbol!=0x29)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;	
						 }	 
					}
					break;

					case 3:	//���� ����� �� ������, �� �������//NEW
					{
						if(symbol!=ADRESS_DEV)//���� ����� ������	  
						{
							PROTO_STATE=PROTO_RESTART;		//����� ���� ����� �������		
						}
					}
					break;
	
					default:  //
					{					
					}
					break;
				}

			RecieveBuf[recieve_count]=symbol;//��������� �������� ������ � �����
			recieve_count++;//������� ������

			if(recieve_count==6)
			{
				frame_len=RecieveBuf[recieve_count-1]; //������� ���������� �����
			}
		}
//---------------------------------------------------------
		else  //������ ����� ����� ����� � ������� ���� ����� 0xD7
		{							
			switch(symbol)//��������, ��� 0x0 ,0xD7 ��� ������
			{
				case 0xD7:
				{
					CUT_OUT_NULL=1;	//
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;
						
				}
				break;

				case 0x0:
				{
					if(!CUT_OUT_NULL)  //���� �������� 0x0 �� ����, �� �� ��������
					{
						RecieveBuf[recieve_count]=symbol;
						recieve_count++;								
					}		
					else //����� � ����� �� ������, ������� ����
					{
						CUT_OUT_NULL=0;			
					}	
				}
				break;

				default:  //������ ������ � �����
				{			
					CUT_OUT_NULL=0;
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;
				}
				break;
			}

			if(recieve_count>=frame_len+6)//���� �������  ��� �����
			{
				PROTO_STATE=PROTO_CRC_CHECK;//PROTO_ADDR_CHECK;
				//----tic stop--------
			//	TIMECON&=0x0;
				//------------
			  	ES=0;
			  	REN=0;  //recieve disable -��������� ��������� � �����		
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
			PROTO_STATE=PROTO_RESTART;
			CUT_OUT_NULL=0;
			DE_RE=0;//����� �� �����
			
		}					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
/*void TIC_ISR(void) interrupt 10 	//using 1   //���������� ������� � 1 �������
{
//EA=0;
	PROTO_STATE=PROTO_RESTART;//���� �������-������������� ��������
	TIMECON&=0x0;
//EA=1;
	return;
}*/ 
//------------------------------------------------------------------------------
void Protocol_Init(void) //using 0
{
	TI=0;
	RI=0;
	//------------------------����� ������--------------------------------
	
	CRC_ERR=0x0;	//������ crc
	COMMAND_ERR=0x0;//���������������� �������
	
	TransferBuf=&RecieveBuf[0];	 //����� ������ =����� �������

//	TIC_Init();

	Restore_Dev_Address_Desc();

	PROTO_STATE=PROTO_RESTART;//������� ���������
	recieve_count=0x0;//������� ������ ������
	transf_count=0x0;//������� ������������ ������
	buf_len=0x0;//����� ������������� ������
	DE_RE=0;//����� �� �����
	CUT_OUT_NULL=0;
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //������� ���������� �� ����������
{
	    unsigned char   idata i=0;
	   									
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

	   TransferBuf[32]=CHANNEL_NUMBER;		   // ���������� �������

	   for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� ������
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ���� ������
		  	TransferBuf[i*2+33+1]=0x00;							// ������ ����
	   }	
	   for(i=0;i<dev_desc_len;i++)					// ���������� ����������
	   {
			 TransferBuf[i+33+CHANNEL_NUMBER*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+CHANNEL_NUMBER*2+dev_desc_len;			// ������� ����� ������ 
	   TransferBuf[33+CHANNEL_NUMBER*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+CHANNEL_NUMBER*2+dev_desc_len); // ������� ����������� �����

	return (34+CHANNEL_NUMBER*2+dev_desc_len);
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
         unsigned char xdata index=0, store_data=0;//i=0;
//	LED=1;	 
	   while(index<RecieveBuf[5]-1)				   // ������ �� �������
	      {
			  	if(RecieveBuf[6+index]<CHANNEL_NUMBER)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case 0x0://���
							{
								if((channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2]) || (channels[RecieveBuf[6+index]].settings.set.state_byte_2!=RecieveBuf[6+index+3]))
								{  
									channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									channels[RecieveBuf[6+index]].settings.set.state_byte_2=RecieveBuf[6+index+3];
									store_data=1;
									
								}
								index=index+1;
							}
							break;

							case 0x2://����������
							{
							   if(channels[RecieveBuf[6+index]].settings.set.state_byte_1!=RecieveBuf[6+index+2])
							   {
							   		channels[RecieveBuf[6+index]].settings.set.state_byte_1=RecieveBuf[6+index+2];
									store_data=1;
							   }
							}
							break;			
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
//	   LED=0;
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
    for(i=0;i<CHANNEL_NUMBER;i++)				   // ������ �� �������
    {
		  TransferBuf[index+7]=i;
		  index++;
		  TransferBuf[index+7]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // ��� � ����������� ������
		  index++;
		  switch(channels[i].settings.set.type)
		    {
				 case 0:  //���������� �����
				 {
					 switch(channels[i].settings.set.modific)
	                 {
						  case 0:
						  {
						  }
						  break; 

						  case 1:
						  {
						  }
						  break;

			        	  case 2: 
						  {
						  }
						  break;

						  case 3:
						  {
						          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // ������ � ���
						          index++;
								  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
								  index++;

								 // if(channels[i].modific==0x02|channels[i].modific==0x03)		   // ���� ������-� 2 � 3 �.�. 24-��������� �������� �� 3 ����� �� �������� 
								  //{
		    					  TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
								  index++;
		  						  //}

								  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
		                          index++;
		                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 // ������ ���� ��������� ������
			                      index++;
						  }
						  break;
					  }
				  }
				  break;

			 	case 1:	 //���
				{
					  switch(channels[i].settings.set.modific)
				      {	  
							  case 0:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // 
							          index++;

									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;

							          TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16; // 
							          index++;

									  TransferBuf[index+7]=((channels[i].channel_data)&0xFF000000)>>24;
									  index++;


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
			                          index++;
							  }
							  break; 
					   }
				}
				break;

				 case 2: //���������
				 { 
					  switch(channels[i].settings.set.modific)
				      {	  
							  case 1:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // ������ � ���
							          index++;
									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;
									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // ������ ���� ��������� ������
			                          index++;
							  }
							  break; 
					   }
				  }
				  break;		 
		  }
	   }

	  TransferBuf[5]=index+2; 						 // ������� ����� ������ ������ ��� 1(���� �������)+1(����������� �����)
	  TransferBuf[index+7]=CRC_Check(&TransferBuf[1],(unsigned int)(index+7)-1); // ������� ��
	  return (unsigned char)(7+index+1);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Calibrate(void)//���������� ������� ��� ������ ����� ����������
{
	if(RecieveBuf[5]==0)//���� ������ �����
	{
//		SetFirstPoint(RecieveBuf[4],ulongsort(adc_channels[RecieveBuf[4]].ADC_BUF_UN.ADC_LONG,ADC_BUF_SIZE),0x0);
	}
	else  //������� �����
	{
//		SetSecondPoint(RecieveBuf[4],ulongsort(adc_channels[RecieveBuf[4]].ADC_BUF_UN.ADC_LONG,ADC_BUF_SIZE),0xFFFFFF);
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
    default:
	{
       COMMAND_ERR=0x1;//�������������� �������
	   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
	   PROTO_STATE=PROTO_ERR_HANDLING;//�� ���������� ������
    }								   
  }

  return;
}
//-----------------------------------------------------------------------------------
void ProtoProcess(void) //������� ������� 
{
	switch(PROTO_STATE)
	{
//------------------------------------------
		case(PROTO_RESTART):
		{
			recieve_count=0x0;//??
		//	transf_count=0x0;
			//TIC_Stop();	 //���������� ��������� �������


			PROTO_STATE=PROTO_WAIT;//��������� � ��������� �������� 

			REN=1;//recieve enqble
			DE_RE=0;//����� �� �����
			ES=1;	//���������� uart	
		}
		break;
//------------------------------------------
		case(PROTO_WAIT)://�������� ������
		{		
			//�����
		}
		break;
//------------------------------------------
	/*	case(PROTO_ADDR_CHECK):// �������� ������	   ��������
		{						
				if(RecieveBuf[3]==ADRESS_DEV)//���� ����� ������	  
				{
					PROTO_STATE=PROTO_CRC_CHECK;	//��������� crc			
				}
				else
				{
					PROTO_STATE=PROTO_RESTART;		//����� ���� ����� �������
				}						
		}
		break; */
//------------------------------------------
		case(PROTO_CRC_CHECK):// �������� CRC
		{		 	
			  unsigned char xdata CRC=0x0;
					
			CRC=RecieveBuf[recieve_count-1];
					
			if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))==CRC)
			{
				PROTO_STATE=PROTO_BUF_HANDLING;	 
			}
			else
			{		
				CRC_ERR=0x1;//������������ CRC
	   			PROTO_STATE=PROTO_ERR_HANDLING;//�� ���������� ������								
			} 	
		}
		break;
//------------------------------------------
		case(PROTO_BUF_HANDLING):// ��������� ��������� ������
		{			
				ProtoBufHandling();//��������� ��������� ���������	
				if(buf_len)//���� � ������ ��� �� ����
					PROTO_STATE=PROTO_BUF_TRANSFER;//����� ��������� �������� ���������	
				else
					PROTO_STATE=PROTO_RESTART; //����� ������� ���������		
		}
		break;
//------------------------------------------
		case(PROTO_BUF_TRANSFER):// �������� ������
		{
				DE_RE=1; //����������� RS485 � ����� ��������
								
				REN=0;	//������ ������-������ ��������
				transf_count=0;
				SBUF=TransferBuf[transf_count];//�������� �����, ��������� �������� �������
				transf_count++;//�������������� ������� ����������
				PROTO_STATE=PROTO_WAIT;//��������� ������ � ������� � ����
				
				ES=1; //������� ���������� ����	
		}
		break;
//------------------------------------------
		case(PROTO_ERR_HANDLING):// ��������� ������ // ����� �� �������������
		{
				//---��������� ������----
				if(CRC_ERR)
				{
					//�������� ��������� ������

					CRC_ERR=0x0;	//������ lrc
				}

				if(COMMAND_ERR)
				{

					COMMAND_ERR=0x0;//���������������� �������	
				}
				//-----------------------
				
				
				PROTO_STATE=PROTO_RESTART;		//����� ������� � ���� ����� �������			
		}
		break;
//------------------------------------------
		default:
		{
			PROTO_STATE==PROTO_RESTART;
		}
		break;
	}

	return;
}
//-----------------------CRC------------------------------------------------------------
static unsigned char  CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr )  //���������
{
    unsigned char   xdata CRC = 0;
	 	unsigned char xdata *Spool; 
	 	unsigned char xdata Count ;
	
	Spool=Spool_pr;
	Count=Count_pr;

  		while(Count!=0x0)
        {
	        CRC = CRC ^ (*Spool++);//
	        // ����������� ����� ������
	        CRC = ((CRC & 0x01) ? (unsigned char)0x80: (unsigned char)0x00) | (unsigned char)(CRC >> 1);
	        // �������� ����� � 2 �� 5, ���� ��� 7 ����� 1
	        if (CRC & (unsigned char)0x80) CRC = CRC ^ (unsigned char)0x3C;
			Count--;
        }
    return CRC;
}

//-----------------------------------------------------------------------------------------------
void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len)//��������� � ���� ����� ����� ����������, ���, ������, ��������
{
//������������
	
	unsigned int blocks=0;//����� ����� ���������� ������, ������ �� ���������� �����������
//LED=1;	
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
//LED=0;	
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
