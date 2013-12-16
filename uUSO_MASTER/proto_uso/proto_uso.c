#include "proto_uso.h"
#include "calibrate\calibrate.h"
#include "i2c.h"
#include <intrins.h>
#include "crc_table.h"
#include "channels.h"
#include "watchdog.h"
//-----------------------------------------------------------------------------------

sbit DE_RE=P3^5;

//-----------------------------------------------------------------------------------
volatile unsigned char xdata DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //имя устройства
volatile unsigned char xdata NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2012 -->";//примечание 	
volatile unsigned char xdata VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// версия программы ПЗУ	не больше 5 байт

volatile unsigned char xdata ADRESS_DEV=0x1;

volatile unsigned char xdata dev_desc_len=20;//длина описания устройства
//--------------------------------global variable------------------------------------
volatile unsigned char idata	RECIEVED=0;//принято
volatile unsigned char xdata    recieve_count;//счетчик приемного буфера
volatile unsigned char xdata	transf_count;//счетчик передаваемых байтов	   
volatile unsigned char xdata	buf_len;//длина передаваемого буфера

//------------------------флаги ошибок--------------------------------
volatile unsigned char idata  CUT_OUT_NULL;//флаг-вырезаем 0 после 0xD7
volatile unsigned char xdata frame_len=0;//длина кадра, которую вытаскиваем из шестого байта кадра
//--------------------------------------------------------------------
volatile unsigned char xdata  RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //буфер принимаемых данных
//volatile unsigned char xdata 			*TransferBuf;
volatile unsigned char xdata  TransferBuf[MAX_LENGTH_TR_BUF]={0} ; //буфер передаваемых данных
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//байт состояния устройства
volatile unsigned char idata symbol=0xFF;//принятый символ

volatile struct pt pt_proto;
//-----------------------------------------------------------------------------------
union //объединение для конвертирования char->long
{
	float result_float;
	unsigned char result_char[4];
}
sym_8_to_float;

extern unsigned char idata i2c_buffer[6];
extern unsigned char channel_number;//количество каналов

//-----------------------------------------------------------------------------------
//#pragma OT(0,Speed)
void UART_ISR(void) interrupt 4 //using 1
{	
	EA=0;	//запрет прерывания
	
	if(RI)
	{
		RI=0; 
//----------------------обрабатываем возможные ошибки длины кадра-------------
		if(recieve_count>MAX_LENGTH_REC_BUF)	//если посылка слишком длинная
		{
			PT_RESTART_OUT(pt_proto);  //внепроцессный рестарт
			return;
		} 


		symbol=SBUF;
		wdt_count[Proto_Proc].process_state=RUN;
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
 				if(CUT_OUT_NULL==1)	  //если после 0xD7-пропускаем
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
	   		  if(recieve_count==6+frame_len)	  // принимаем указанное в frame_len число байт данные, 6 значит что обмен идет с компом, надо ставаить 5 чтобы обмениваться с устройствами
   			  {
					RECIEVED=1;//буфер принят
			  		ES=0;
			  		REN=0;  //recieve disable -запрещаем принимать в буфер	
				   	CUT_OUT_NULL=0;  			  			
			  }	  
	   }
	   else
	   {
			   if(recieve_count==6)
			   {     
		        	frame_len=RecieveBuf[recieve_count-1];  // получаем длину данных после заголовка					 
			   }	   		
	   }										
	}
//----------------------------передача----------------------------------------------------------------
	if(TI)
	{
		TI=0;
		 
		if(transf_count<buf_len)
		{
			if(transf_count<3)//передаем заголовок
			{
				SBUF=TransferBuf[transf_count];			
				transf_count++;
			}
			else   //тело...   подставляем 0 после 0xD7
			{
					if(CUT_OUT_NULL==0)
					{
						if(TransferBuf[transf_count]==(unsigned char)0xD7)//проверим, это  ,0xD7 или другое
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
			transf_count=0;		//обнуляем счетчик
			CUT_OUT_NULL=0;
			PT_RESTART_OUT(pt_proto);  //внепроцессный рестарт			
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
	
//	TransferBuf=&RecieveBuf[0];	 //буфер ответа =буфер запроса

	Restore_Dev_Address_Desc();


	recieve_count=0x0;//счетчик буфера приема
	transf_count=0x0;//счетчик передаваемых байтов
	buf_len=0x0;//длина передаваемого буфера
	DE_RE=0;//линия на прием
	CUT_OUT_NULL=0;
	STATE_BYTE=0xC0;
	PT_INIT(&pt_proto);
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //посылка информации об устройстве
{
	    unsigned char    i=0;
	   									
	   //заголовок кадра---
	   TransferBuf[0]=0x00;
	   TransferBuf[1]=0xD7;
	   TransferBuf[2]=0x29;
	   //------------------
	   TransferBuf[3]=ADRESS_DEV;  // адрес узла
	   TransferBuf[4]=GET_DEV_INFO_RESP;  // код операции
	   TransferBuf[6]=STATE_BYTE;

	   for(i=0;i<20;i++)
	   {				  // записываем наименование изделия
			   if(i<DEVICE_NAME_LENGTH_SYM)
			   {
			     	TransferBuf[i+7]=DEV_NAME[i];
			   }
			   else
			   {
			   		TransferBuf[i+7]=0x00;
			   }
		}
	
	   for(i=0;i<5;i++)                   // записываем версию ПЗУ
	   {
	       if(i<DEVICE_VER_LENGTH_SYM)
		   {
		    	 TransferBuf[i+27]=VERSION[i];
		   }
	   }

	   TransferBuf[32]=channel_number;		   // количество каналов

	   for(i=0;i<channel_number;i++)				   // данные по каналу
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // байт данных
		  	TransferBuf[i*2+33+1]=0x00;							// резерв байт
	   }	
	   for(i=0;i<dev_desc_len;i++)					// записываем примечание
	   {
			 TransferBuf[i+33+channel_number*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+channel_number*2+dev_desc_len;			// подсчет длины данных 
	   TransferBuf[33+channel_number*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+channel_number*2+dev_desc_len); // подсчет контрольной суммы

	return (34+channel_number*2+dev_desc_len);
}
//-----------------------------------------------------------------------------
unsigned char Node_Full_Init(void) //using 0 //полная инициализация узла
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_List_Init(void) //using 0 //Инициализация списка каналов узла (без потери данных);
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data(void) //using 0 //Выдать данные по каналам, согласно абсолютной нумерации;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Set_Parameters(void) //using 0 //Установить параметры по каналам, согласно абсолютной нумерации;
{
       unsigned char  index=0, store_data=0;//i=0;
	 
	   while(index<RecieveBuf[5]-1)				   // данные по каналам
	      {
			  	if(RecieveBuf[6+index]<channel_number)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case CHNL_ADC://АЦП
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
	   		Store_Channels_Data();	//сохраним настройки каналов в ППЗУ
	   }

	   return Request_Error(FR_SUCCESFUL);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Order_Query(void) //using 0 //Задать последовательность опроса;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Get_Data_Order(void) //using 0 //Выдать данные по каналам, согласно последовательности опроса;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_State(void) //using 0 //Установить состояния по каналам, согласно абсолютной нумерации;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char  Channel_Get_Data_Order_M2(void) //using 0 //Выдать данные по каналам, согласно последовательности опроса;
{
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Reset_State_Flags(void) //using 0 //	Установка/Сброс флагов состояния 
{
	STATE_BYTE=0x40;
	i2c_buffer[0]=0x12;//сбросим флаг инициализации ведомого
	return	Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение
}
//-----------------------------------------------------------------------------
#pragma OT(9,Speed)
unsigned char Channel_All_Get_Data(void) //using 0 //Выдать информацию по всем каналам узла (расширенный режим);
{
   unsigned char data index=0,i=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // адрес узла
   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  // код операции
   TransferBuf[6]=STATE_BYTE;

   index=7;//длина заголовка
 
    for(i=0;i<channel_number;i++)				   // данные по каналам
    {
		  TransferBuf[index++]=i;
		  TransferBuf[index++]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // тип и модификация канала
		  switch(channels[i].settings.set.type)
		    {
				 case CHNL_ADC:  //аналоговый канал
				 {
					 switch(channels[i].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[i].calibrate.cal.calibrate==1)//калиброванный
								{			 			 
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data_calibrate))[1];
								}
								else
								{
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[1];	
								} 

								  
								 TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
		                         TransferBuf[index++]=channels[i].settings.set.state_byte_2;	 // второй байт состояния канала
						  }
						  break; 

						  case CHNL_ADC_FIX_24:
						  {
						        if(channels[i].calibrate.cal.calibrate==1)//калиброванный
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
		  
								  TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
		                          TransferBuf[index++]=channels[i].settings.set.state_byte_2;	 // второй байт состояния канала
						  }
						  break;
					  }
				  }
				  break;

			 	case CHNL_DOL:	 //ДОЛ
				{
					  switch(channels[i].settings.set.modific)
				      {	  
							  case CHNL_DOL_ENC:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
			    					  	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[1];
										TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[0];
									    TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
							  }
							  break; 
					   }
				}
				break;

				 case CHNL_FREQ: //частотный
				 { 
					  switch(channels[i].settings.set.modific)
				      {	  
							  
							  case CHNL_FREQ_COUNT_T:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
									  TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
							  }
							  break;

							  case CHNL_FREQ_256:
							  {
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[3];
									 	TransferBuf[index++]=((unsigned char*)(&channels[i].channel_data))[2];
									    TransferBuf[index++]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
							  }
							  break; 
					   }
				  }
				  break;		 
		  }
	   }

	  TransferBuf[5]=index+2-7; 						 // подсчет длины данных двойка это 1(байт статуса)+1(контрольная сумма)
	  TransferBuf[index]=CRC_Check(&TransferBuf[1],(unsigned int)(index)-1); // подсчет кс
	  return (unsigned char)(index+1);
}
//-----------------------------------------------------------------------------
#pragma OT(0,Speed)
unsigned char Channel_Set_Calibrate(void)//установить верхнюю или нижнюю точку калибровки
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


	return	Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение	
}
//------------------------------------------------------------------------------
unsigned char Channel_Get_Calibrate_Value(void)//получить коэфициенты калибровки заданного канала
{
   unsigned char data channel=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // адрес узла
   TransferBuf[4]=CHANNEL_GET_CALIBRATE_RESP;  // код операции
   TransferBuf[5]=0xB;//длина оставшейся части кадра


   channel=RecieveBuf[6];

   if(channel>=channel_number)
   {
   		return Request_Error(FR_COMMAND_STRUCT_ERROR);//непрвильный номер канала	
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

	TransferBuf[16]=CRC_Check(&TransferBuf[1],15); // подсчет кс
	return 0x11;//17	
}
//------------------------------------------------------------------------------
unsigned char Channel_Set_Address_Desc(void)//установить новый адрес устройства, имя, описание, версию прошивки и комментарий
{
	unsigned char  desc_len=0;
	desc_len=RecieveBuf[5]-27;

	if(desc_len)
	{	
   		Store_Dev_Address_Desc(RecieveBuf[6],&RecieveBuf[7],&RecieveBuf[27],&RecieveBuf[32],desc_len);
		return 	  Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение
	}
	else
	{
		return Request_Error(FR_COMMAND_STRUCT_ERROR);
	}
	return 0;
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_All_Default(void)//установить настройки и калибровки каналов по умолчанию
{
	Channels_Set_Default();
	Calibrate_Set_Default();
	return	Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение		
}
//-----------------------------------------------------------------------------
unsigned char Request_Error(unsigned char error_code) //using 0 //	Ошибочный запрос/ответ;
{
	TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
    TransferBuf[3]=ADRESS_DEV;  // адрес узла
    TransferBuf[7]=RecieveBuf[4]; // код сбойной команды
    TransferBuf[4]=0xFF;  // код операции

	TransferBuf[6]=STATE_BYTE; // байт статуса узла
    TransferBuf[8]=error_code;	 
    TransferBuf[5]=0x04;	  // длина данных
    TransferBuf[9]=CRC_Check(TransferBuf,9);
	return 10;
}
//-----------------------------------------------------------------------------
void ProtoBufHandling(void) //using 0 //процесс обработки принятого запроса
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

  return;
}
//-----------------------------------------------------------------------------------
//#pragma OT(0,Speed) 
PT_THREAD(ProtoProcess(struct pt *pt))
 {
 static unsigned char  CRC=0x0;
  PT_BEGIN(pt);

  while(1) 
  {
  //----------restart------------
		recieve_count=0x0;//??
		REN=1;//recieve enqble
		DE_RE=0;//линия на прием
		ES=1;
  //-----------------------------
	  // PT_WAIT_UNTIL(pt,RECIEVED); //ждем команды на старт
	   wdt_count[Proto_Proc].process_state=IDLE;

	   PT_YIELD_UNTIL(pt,RECIEVED); //ждем команды на старт	
	   wdt_count[Proto_Proc].count++;
	  // WDT_Clear();//если посылка не приходит-сбрасываем

	  
	    RECIEVED=0;
		
		if(RecieveBuf[3]!=ADRESS_DEV)//если адрес совпал	  
		{
			PT_RESTART(pt);//если адрес не сошелся-перезапустим протокол			
		}	
				
	    CRC=RecieveBuf[recieve_count-1];
				
		if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
		{
			PT_RESTART(pt);//если CRC не сошлось-перезапустим протокол	 
		}
		PT_YIELD(pt);//дадим другим процессам время
  //-----------------------------
  		ProtoBufHandling();//процедура обработки сообщения	
		if(buf_len==0)//если в буфере пусто
		{
			PT_RESTART(pt);//перезапустим протокол	
		}
		else
		{
			DE_RE=1; //переключаем RS485 в режим передачи
							
			REN=0;	//запрет приема-только передача
			transf_count=0;
			CUT_OUT_NULL=0;
			SBUF=TransferBuf[transf_count];//передача байта, остальным займется автомат
			transf_count++;//инкрементируем счетчик переданных
			ES=1; //включим прерывание уарт	

			PT_DELAY(pt,10);			
		}

		
  //-----------------------------
  }

 PT_END(pt);
}
//-----------------------------------------------------------------------------------------------
void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len)//сохранить в ППЗУ новый адрес устройства, имя, версию, описание
{
//небезопасная
	
	unsigned int blocks=0;//здесь будет количество блоков, нужных дя сохранения комментария
	
	EEPROM_Write(&addr,1,DEVICE_ADDR_EEPROM);	 //1 блок
	EEPROM_Write(name,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 блоков
	memcpy(DEV_NAME,name,DEVICE_NAME_LENGTH_SYM);//копируем полученное имя в буфер

	EEPROM_Write(ver,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);		//2 блока
	memcpy(VERSION,ver,DEVICE_VER_LENGTH_SYM);//копируем версию в буфер

	blocks=desc_len>>2;//в блоке 4 байта, делим на 4
	if(desc_len&0xFC)//если есть остаток, то берем еще один блок
		blocks++;
	
	if(blocks>DEVICE_DESC_MAX_LENGTH) //ограничение на количество блоков для комментария
		blocks=DEVICE_DESC_MAX_LENGTH;

	EEPROM_Write(desc,(unsigned int)blocks,DEVICE_DESC_EEPROM);
	dev_desc_len=desc_len;
	EEPROM_Write(&desc_len,1,DEVICE_DESC_LEN_EEPROM);//сохраним длину комментария
	
	memcpy(NOTICE,desc,desc_len);//копируем описание в буфер
	
	return;
}
//-----------------------------------------------------------------------------------------------
void Restore_Dev_Address_Desc(void)//восстановить из ппзу адрес и информацию об устройстве
{
	unsigned int blocks=0;//здесь будет количество блоков, нужных дя сохранения комментария
	
	EEPROM_Read(&ADRESS_DEV,1,DEVICE_ADDR_EEPROM);	 //1 блок восстанавливаем адрес из ППЗУ

	if(ADRESS_DEV<1 || ADRESS_DEV>15)
		ADRESS_DEV=1; 

	EEPROM_Read(DEV_NAME,DEVICE_NAME_LENGTH,DEVICE_NAME_EEPROM); //5 блоков
    EEPROM_Read(VERSION,DEVICE_VER_LENGTH,DEVICE_VER_EEPROM);

	EEPROM_Read(&dev_desc_len,1,DEVICE_DESC_LEN_EEPROM);//длина описания устройства в байтах

	if(dev_desc_len>DEVICE_DESC_MAX_LENGTH_SYM)
		dev_desc_len=DEVICE_DESC_MAX_LENGTH_SYM;

	blocks=dev_desc_len>>2;//в блоке 4 байта, делим на 4
	if(dev_desc_len&0xFC)//если есть остаток, то берем еще один блок
		blocks++;

	if(blocks>DEVICE_DESC_MAX_LENGTH) //ограничение на количество блоков для комментария	 ???
		blocks=DEVICE_DESC_MAX_LENGTH;

	EEPROM_Read(NOTICE,(unsigned int)blocks,DEVICE_DESC_EEPROM);
	return;
}
//-----------------------------------------------------------------------------------------------
