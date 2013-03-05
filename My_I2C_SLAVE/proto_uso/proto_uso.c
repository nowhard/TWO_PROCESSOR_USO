#include "proto_uso.h"

sbit DE_RE=P3^5;
 sbit LED=P3^7;
//-----------------------------------------------------------------------------------
volatile unsigned char xdata DEV_NAME[DEVICE_NAME_LENGTH_SYM] ="<<uUSO_2>>"; //имя устройства
volatile unsigned char xdata NOTICE[DEVICE_DESC_MAX_LENGTH_SYM]="<-- GEOSPHERA_2011 -->";//примечание 	
volatile unsigned char xdata VERSION[DEVICE_VER_LENGTH_SYM] ="\x30\x30\x30\x30\x31";	// версия программы ПЗУ	не больше 5 байт

volatile unsigned char xdata ADRESS_DEV=0x1;

volatile unsigned char xdata dev_desc_len=20;//длина описания устройства
//--------------------------------global variable------------------------------------
volatile unsigned char xdata	PROTO_STATE;//счетчик состояний
volatile unsigned char xdata    recieve_count;//счетчик приемного буфера
volatile unsigned char xdata	transf_count;//счетчик передаваемых байтов	   
volatile unsigned char xdata	buf_len;//длина передаваемого буфера

//------------------------флаги ошибок--------------------------------
volatile unsigned char xdata CRC_ERR;	//ошибка сrc
volatile unsigned char xdata COMMAND_ERR;//неподдерживаемая команда

volatile unsigned char xdata TIMEOUT;//таймаут 

volatile unsigned char idata  CUT_OUT_NULL;//флаг-вырезаем 0 после 0xD7
volatile unsigned char xdata frame_len=0;//длина кадра, которую вытаскиваем из шестого байта кадра
//--------------------------------------------------------------------
volatile unsigned char xdata  RecieveBuf[MAX_LENGTH_REC_BUF]={0} ; //буфер принимаемых данных
volatile unsigned char xdata 			*TransferBuf;
//static unsigned char /*data*/ volatile  TransferBuf[MAX_LENGTH_TR_BUF] ; //буфер передаваемых данных
//--------------------------------------------------------------------
volatile unsigned char xdata  STATE_BYTE=0xC0;//байт состояния устройства
//-----------------------------------------------------------------------------------

void UART_ISR(void) interrupt 4 //using 1
{	
  unsigned char idata symbol=0xFF;//принятый символ

	EA=0;	//запрет прерывания
	
	if(RI)
	{
		RI=0; 
//----------------------обрабатываем возможные ошибки длины кадра-------------
		if(recieve_count>MAX_LENGTH_REC_BUF)	//если посылка слишком длинная
		{
			PROTO_STATE=PROTO_RESTART;//делаем рестарт протокола
			return;
		} 
		//TIC_Start(); //перезапуск таймера таймаута

		//------tic start-------
		//TIMECON&=0x0;
	  	//INTVAL=0x1;
	  //	TIMECON=0x1B;//00011011 -один период, прерывание 1 секунда	
		//-------------


		symbol=SBUF;
		
//--------------------------начало кадра...проверка до длины кадра--------
	    if(recieve_count<6)
		{
				switch(recieve_count)	 
				{
					case  0:   //первый символ 0
					{
	 				 	 if(symbol!=0x00)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;								 	
						 }	 
					}
					break;
	
					case 1:	 //второй символ 0xD7
					{
						 if(symbol!=0xD7)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;
						 }		 
					}
					break;
	
					case 2:	 //	третий символ 0x29
					{
					 	 if(symbol!=0x29)
						 {
						 	PROTO_STATE=PROTO_RESTART;
							EA=1;
							return;	
						 }	 
					}
					break;

					case 3:	//если адрес не совпал, то сбросим//NEW
					{
						if(symbol!=ADRESS_DEV)//если адрес совпал	  
						{
							PROTO_STATE=PROTO_RESTART;		//иначе ждем новой посылки		
						}
					}
					break;
	
					default:  //
					{					
					}
					break;
				}

			RecieveBuf[recieve_count]=symbol;//сохраняем принятый символ в буфер
			recieve_count++;//счетчик буфера

			if(recieve_count==6)
			{
				frame_len=RecieveBuf[recieve_count-1]; //получим оставшуюся длину
			}
		}
//---------------------------------------------------------
		else  //отсюда знаем длину кадра и удаляем нули после 0xD7
		{							
			switch(symbol)//проверим, это 0x0 ,0xD7 или другое
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
					if(!CUT_OUT_NULL)  //если вырезать 0x0 не надо, то не вырезаем
					{
						RecieveBuf[recieve_count]=symbol;
						recieve_count++;								
					}		
					else //иначе в буфер не кладем, сбросим флаг
					{
						CUT_OUT_NULL=0;			
					}	
				}
				break;

				default:  //другие кладем в буфер
				{			
					CUT_OUT_NULL=0;
					RecieveBuf[recieve_count]=symbol;
					recieve_count++;
				}
				break;
			}

			if(recieve_count>=frame_len+6)//если приняты  все байты
			{
				PROTO_STATE=PROTO_CRC_CHECK;//PROTO_ADDR_CHECK;
				//----tic stop--------
			//	TIMECON&=0x0;
				//------------
			  	ES=0;
			  	REN=0;  //recieve disable -запрещаем принимать в буфер		
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
			PROTO_STATE=PROTO_RESTART;
			CUT_OUT_NULL=0;
			DE_RE=0;//линия на прием
			
		}					   
	}			
	EA=1;
	return;
}
//------------------------------------------------------------------------------
/*void TIC_ISR(void) interrupt 10 	//using 1   //прерывания таймера в 1 секунду
{
//EA=0;
	PROTO_STATE=PROTO_RESTART;//если таймаут-перезагружаем протокол
	TIMECON&=0x0;
//EA=1;
	return;
}*/ 
//------------------------------------------------------------------------------
void Protocol_Init(void) //using 0
{
	TI=0;
	RI=0;
	//------------------------флаги ошибок--------------------------------
	
	CRC_ERR=0x0;	//ошибка crc
	COMMAND_ERR=0x0;//неподдерживаемая команда
	
	TransferBuf=&RecieveBuf[0];	 //буфер ответа =буфер запроса

//	TIC_Init();

	Restore_Dev_Address_Desc();

	PROTO_STATE=PROTO_RESTART;//счетчик состояний
	recieve_count=0x0;//счетчик буфера приема
	transf_count=0x0;//счетчик передаваемых байтов
	buf_len=0x0;//длина передаваемого буфера
	DE_RE=0;//линия на прием
	CUT_OUT_NULL=0;
	return;
}
//-----------------------------------------------------------------------------
unsigned char Send_Info(void) //using 0    //посылка информации об устройстве
{
	    unsigned char   idata i=0;
	   									
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

	   TransferBuf[32]=CHANNEL_NUMBER;		   // количество каналов

	   for(i=0;i<CHANNEL_NUMBER;i++)				   // данные по каналу
       {
		  	TransferBuf[i*2+33]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // байт данных
		  	TransferBuf[i*2+33+1]=0x00;							// резерв байт
	   }	
	   for(i=0;i<dev_desc_len;i++)					// записываем примечание
	   {
			 TransferBuf[i+33+CHANNEL_NUMBER*2]=NOTICE[i];
	   }
			
	   TransferBuf[5]=28+CHANNEL_NUMBER*2+dev_desc_len;			// подсчет длины данных 
	   TransferBuf[33+CHANNEL_NUMBER*2+dev_desc_len]=CRC_Check(&TransferBuf[1],32+CHANNEL_NUMBER*2+dev_desc_len); // подсчет контрольной суммы

	return (34+CHANNEL_NUMBER*2+dev_desc_len);
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
         unsigned char xdata index=0, store_data=0;//i=0;
//	LED=1;	 
	   while(index<RecieveBuf[5]-1)				   // данные по каналам
	      {
			  	if(RecieveBuf[6+index]<CHANNEL_NUMBER)
			    {
					switch((RecieveBuf[6+index+1]>>4)&0xF)
					{
					 		case 0x0://АЦП
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

							case 0x2://частотомер
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
	   		Store_Channels_Data();	//сохраним настройки каналов в ППЗУ
		}
//	   LED=0;
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
	return	Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение
}
//-----------------------------------------------------------------------------
unsigned char Channel_All_Get_Data(void) //using 0 //Выдать информацию по всем каналам узла (расширенный режим);
{
   unsigned char data index=0,i=0;


   TransferBuf[0]=0x00;TransferBuf[1]=0xD7;TransferBuf[2]=0x29;
   TransferBuf[3]=ADRESS_DEV;  // адрес узла
   TransferBuf[4]=CHANNEL_ALL_GET_DATA_RESP;  // код операции
   TransferBuf[6]=STATE_BYTE;
    for(i=0;i<CHANNEL_NUMBER;i++)				   // данные по каналам
    {
		  TransferBuf[index+7]=i;
		  index++;
		  TransferBuf[index+7]=((channels[i].settings.set.type)<<4)|channels[i].settings.set.modific; // тип и модификация канала
		  index++;
		  switch(channels[i].settings.set.type)
		    {
				 case 0:  //аналоговый канал
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
						          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // данные с АЦП
						          index++;
								  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
								  index++;

								 // if(channels[i].modific==0x02|channels[i].modific==0x03)		   // если модифи-я 2 и 3 т.е. 24-разрядные значения то 3 байта на значение 
								  //{
		    					  TransferBuf[index+7]=((channels[i].channel_data)&0x00FF0000)>>16;
								  index++;
		  						  //}

								  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
		                          index++;
		                          TransferBuf[index+7]=channels[i].settings.set.state_byte_2;	 // второй байт состояния канала
			                      index++;
						  }
						  break;
					  }
				  }
				  break;

			 	case 1:	 //ДОЛ
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


									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
			                          index++;
							  }
							  break; 
					   }
				}
				break;

				 case 2: //частотный
				 { 
					  switch(channels[i].settings.set.modific)
				      {	  
							  case 1:
							  {
							          TransferBuf[index+7]=((channels[i].channel_data)&0x000000FF); // данные с АЦП
							          index++;
									  TransferBuf[index+7]=((channels[i].channel_data)&0x0000FF00)>>8;
									  index++;
									  TransferBuf[index+7]=channels[i].settings.set.state_byte_1;	 // первый байт состояния канала
			                          index++;
							  }
							  break; 
					   }
				  }
				  break;		 
		  }
	   }

	  TransferBuf[5]=index+2; 						 // подсчет длины данных двойка это 1(байт статуса)+1(контрольная сумма)
	  TransferBuf[index+7]=CRC_Check(&TransferBuf[1],(unsigned int)(index+7)-1); // подсчет кс
	  return (unsigned char)(7+index+1);
}
//-----------------------------------------------------------------------------
unsigned char Channel_Set_Calibrate(void)//установить верхнюю или нижнюю точку калибровки
{
	if(RecieveBuf[5]==0)//если нижняя точка
	{
//		SetFirstPoint(RecieveBuf[4],ulongsort(adc_channels[RecieveBuf[4]].ADC_BUF_UN.ADC_LONG,ADC_BUF_SIZE),0x0);
	}
	else  //верхняя точка
	{
//		SetSecondPoint(RecieveBuf[4],ulongsort(adc_channels[RecieveBuf[4]].ADC_BUF_UN.ADC_LONG,ADC_BUF_SIZE),0xFFFFFF);
	}
	return	Request_Error(FR_SUCCESFUL);//ошибки нет, подтверждение	
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
    default:
	{
       COMMAND_ERR=0x1;//несуществующая команда
	   buf_len=Request_Error(FR_COMMAND_NOT_EXIST);
	   PROTO_STATE=PROTO_ERR_HANDLING;//на обработчик ошибки
    }								   
  }

  return;
}
//-----------------------------------------------------------------------------------
void ProtoProcess(void) //главный процесс 
{
	switch(PROTO_STATE)
	{
//------------------------------------------
		case(PROTO_RESTART):
		{
			recieve_count=0x0;//??
		//	transf_count=0x0;
			//TIC_Stop();	 //остановить секундный таймаут


			PROTO_STATE=PROTO_WAIT;//переходим в состояние ожидания 

			REN=1;//recieve enqble
			DE_RE=0;//линия на прием
			ES=1;	//прерывание uart	
		}
		break;
//------------------------------------------
		case(PROTO_WAIT)://ожидание приема
		{		
			//пусто
		}
		break;
//------------------------------------------
	/*	case(PROTO_ADDR_CHECK):// проверка адреса	   Устарело
		{						
				if(RecieveBuf[3]==ADRESS_DEV)//если адрес совпал	  
				{
					PROTO_STATE=PROTO_CRC_CHECK;	//проверяем crc			
				}
				else
				{
					PROTO_STATE=PROTO_RESTART;		//иначе ждем новой посылки
				}						
		}
		break; */
//------------------------------------------
		case(PROTO_CRC_CHECK):// проверка CRC
		{		 	
			  unsigned char xdata CRC=0x0;
					
			CRC=RecieveBuf[recieve_count-1];
					
			if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))==CRC)
			{
				PROTO_STATE=PROTO_BUF_HANDLING;	 
			}
			else
			{		
				CRC_ERR=0x1;//несовпадение CRC
	   			PROTO_STATE=PROTO_ERR_HANDLING;//на обработчик ошибки								
			} 	
		}
		break;
//------------------------------------------
		case(PROTO_BUF_HANDLING):// обработка принятого буфера
		{			
				ProtoBufHandling();//процедура обработки сообщения	
				if(buf_len)//если в буфере что то есть
					PROTO_STATE=PROTO_BUF_TRANSFER;//после обработки передаем сообщение	
				else
					PROTO_STATE=PROTO_RESTART; //иначе рестарт протокола		
		}
		break;
//------------------------------------------
		case(PROTO_BUF_TRANSFER):// передача буфера
		{
				DE_RE=1; //переключаем RS485 в режим передачи
								
				REN=0;	//запрет приема-только передача
				transf_count=0;
				SBUF=TransferBuf[transf_count];//передача байта, остальным займется автомат
				transf_count++;//инкрементируем счетчик переданных
				PROTO_STATE=PROTO_WAIT;//забросили данные в автомат и ждем
				
				ES=1; //включим прерывание уарт	
		}
		break;
//------------------------------------------
		case(PROTO_ERR_HANDLING):// обработка ошибок // можно не реализовывать
		{
				//---обработка ошибок----
				if(CRC_ERR)
				{
					//написать обработку ошибок

					CRC_ERR=0x0;	//ошибка lrc
				}

				if(COMMAND_ERR)
				{

					COMMAND_ERR=0x0;//неподдерживаемая команда	
				}
				//-----------------------
				
				
				PROTO_STATE=PROTO_RESTART;		//иначе рестарт и ждем новой посылки			
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
static unsigned char  CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr )  //проверить
{
    unsigned char   xdata CRC = 0;
	 	unsigned char xdata *Spool; 
	 	unsigned char xdata Count ;
	
	Spool=Spool_pr;
	Count=Count_pr;

  		while(Count!=0x0)
        {
	        CRC = CRC ^ (*Spool++);//
	        // циклический сдвиг вправо
	        CRC = ((CRC & 0x01) ? (unsigned char)0x80: (unsigned char)0x00) | (unsigned char)(CRC >> 1);
	        // инверсия битов с 2 по 5, если бит 7 равен 1
	        if (CRC & (unsigned char)0x80) CRC = CRC ^ (unsigned char)0x3C;
			Count--;
        }
    return CRC;
}

//-----------------------------------------------------------------------------------------------
void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len)//сохранить в ППЗУ новый адрес устройства, имя, версию, описание
{
//небезопасная
	
	unsigned int blocks=0;//здесь будет количество блоков, нужных дя сохранения комментария
//LED=1;	
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
//LED=0;	
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
