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

volatile unsigned char xdata protocol_type=PROTO_TYPE_NEW;//тип протокола

volatile struct pt pt_proto, pt_buf_handle;

struct RingBuf rngbuf;
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
		rngbuf.buf[rngbuf.tail]=SBUF;
		rngbuf.tail++;
		rngbuf.count++;										
	}
//----------------------------передача----------------------------------------------------------------
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
			transf_count=0;		//обнуляем счетчик
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

	rngbuf.tail=0;
	rngbuf.head=0;
	rngbuf.count=0;

	recieve_count=0x0;//счетчик буфера приема
	transf_count=0x0;//счетчик передаваемых байтов
	buf_len=0x0;//длина передаваемого буфера
	DE_RE=0;//линия на прием
	CUT_OUT_NULL=0;
	STATE_BYTE=0xC0;
	PT_INIT(&pt_proto);
	PT_INIT(&pt_buf_handle);
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
	ChannelsInit();
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
	   TransferBuf[5]=((OLD_CHANNEL_GET_DATA<<5)&0x1F)|0x3;//0x63;// код операции	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //аналоговый канал
				 {
					 switch(channels[channel].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[channel].calibrate.cal.calibrate==1)//калиброванный
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
						        if(channels[channel].calibrate.cal.calibrate==1)//калиброванный
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

			 	case CHNL_DOL:	 //ДОЛ
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

				 case CHNL_FREQ: //частотный
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
	   TransferBuf[5]=((OLD_CHANNEL_GET_DATA_STATE<<5)&0x1F)|0x6;//0x66;// код операции	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //аналоговый канал
				 {
					 switch(channels[channel].settings.set.modific)
	                 {
						  case CHNL_ADC_FIX_16:
						  {
						  		if(channels[channel].calibrate.cal.calibrate==1)//калиброванный
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
						        if(channels[channel].calibrate.cal.calibrate==1)//калиброванный
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
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // первый байт состояния канала
							}
							break;

					  		case PROTO_ADC_AMP_2:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_2<<1))&0xFE;	 // первый байт состояния канала	
							}
							break;

					  		case PROTO_ADC_AMP_32:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_32<<1))&0xFE;	 // первый байт состояния канала	
							}
							break;

					  		case PROTO_ADC_AMP_128:
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_128<<1))&0xFE;	 // первый байт состояния канала
							}
							break;

					  		default :
							{
								TransferBuf[8]=(0x00|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // первый байт состояния канала
							}
							break;
					  }
					  
					  TransferBuf[9]=0x0A;	 // второй байт состояния канала

				  }
				  break;

			  case CHNL_DOL:	 //ДОЛ
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
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;	 // первый байт состояния канала
										}
										break;
			
								  		case PROTO_ADC_AMP_2:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_2<<1))&0xFE;	 // первый байт состояния канала	
										}
										break;
			
								  		case PROTO_ADC_AMP_32:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_32<<1))&0xFE;	 // первый байт состояния канала	
										}
										break;
			
								  		case PROTO_ADC_AMP_128:
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_128<<1))&0xFE;	 // первый байт состояния канала
										}
										break;
			
								  		default :
										{
											TransferBuf[8]=(0x40|(OLD_PROTO_ADC_AMP_1<<1))&0xFE;
										}
										break;			
								}
								TransferBuf[9]=0x00;	 // второй байт состояния канала
						  }
						  break; 
					   }
				}
				break;

				 case CHNL_FREQ: //частотный
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
					   
					TransferBuf[8]=0x40;	 // первый байт состояния канала
       				TransferBuf[9]=0x00;	 // второй байт состояния канала
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
	   TransferBuf[5]=((OLD_CHANNEL_GET_STATE<<5)&0x1F)|0x3;//0x43;// код операции	

 			switch(channels[channel].settings.set.type)
		    {
				 case CHNL_ADC:  //аналоговый канал
				 {		  
					  switch(channels[channel].settings.set.state_byte_1&0x7)
					  {
					  		case PROTO_ADC_AMP_1:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_1<<1);	 // первый байт состояния канала
							}
							break;

					  		case PROTO_ADC_AMP_2:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_2<<1);	 // первый байт состояния канала	
							}
							break;

					  		case PROTO_ADC_AMP_32:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_32<<1);	 // первый байт состояния канала	
							}
							break;

					  		case PROTO_ADC_AMP_128:
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_128<<1);	 // первый байт состояния канала
							}
							break;

					  		default :
							{
								TransferBuf[6]=0x00|(OLD_PROTO_ADC_AMP_1<<1);	 // первый байт состояния канала
							}
							break;
					  }
					  
					  TransferBuf[7]=0x0A;	 // второй байт состояния канала
				  }
				  break;

			 	case CHNL_DOL:	 //ДОЛ
				{

						switch(channels[channel].settings.set.state_byte_1&0x7)
						{
						  		case PROTO_ADC_AMP_1:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_1<<1);	 // первый байт состояния канала
								}
								break;
	
						  		case PROTO_ADC_AMP_2:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_2<<1);	 // первый байт состояния канала	
								}
								break;
	
						  		case PROTO_ADC_AMP_32:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_32<<1);	 // первый байт состояния канала	
								}
								break;
	
						  		case PROTO_ADC_AMP_128:
								{
									TransferBuf[6]=0x40|(OLD_PROTO_ADC_AMP_128<<1);	 // первый байт состояния канала
								}
								break;
	
						  		default :
								{
									TransferBuf[6]=0x40;	 // первый байт состояния канала
								}
								break;
	
						}
						TransferBuf[7]=0x00;	 // второй байт состояния канала
				}
				break;

				 case CHNL_FREQ: //частотный
				 { 
					   
					TransferBuf[6]=0x40;	 // первый байт состояния канала
       				TransferBuf[7]=0x00;	 // второй байт состояния канала
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
unsigned char Proto_Paste_Null(unsigned char *buf,unsigned char len)//т.к. старый протокол прибавляет к длине количество 0 после D7 то обрабатываем вне прерывания
{
	volatile unsigned char xdata i=0,j=0;

	for(i=3;i<len;i++) //пропустим заголовок 0x00 0xD7 0x28
	{
		 if(buf[i]==0xD7)
		 {
		 	 for(j=(len-1);j>i;j--)
			 {
			 	buf[j+1]=buf[j];//сдвигаем элементы буфера вправо на одну позицию до D7
			 }
			 buf[i+1]=0x0;//вставляем 0x00 после 0xD7

			 if(protocol_type==PROTO_TYPE_OLD)
			 {
			 	buf[5]++;//длина кадра увеличилась на 1
			 }

			 len++;
			 i++;//не проверяем 0 спереди
		 }
	}
	return len;
}
//-----------------------------------------------------------------------------
void ProtoBufHandling(void) //using 0 //процесс обработки принятого запроса
{
	switch(protocol_type)
	{
		case PROTO_TYPE_NEW:
		{
			  channels[11].settings.set.state_byte_1=0x40; //восстановим байты статуса дола
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

		case PROTO_TYPE_OLD: //старый протокол
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
		DE_RE=0;//линия на прием
		ES=1;
  //-----------------------------
	  // PT_WAIT_UNTIL(pt,RECIEVED); //ждем команды на старт
	   wdt_count[Proto_Proc].process_state=IDLE;

	   PT_YIELD_UNTIL(pt,RECIEVED); //ждем команды на старт	
	   wdt_count[Proto_Proc].count++;

	    RECIEVED=0;
		

	 	switch(protocol_type)
		{
			case PROTO_TYPE_NEW:
			{
				if(RecieveBuf[3]!=ADRESS_DEV)//если адрес совпал	  
				{
					PT_RESTART(pt);//если адрес не сошелся-перезапустим протокол			
				}

				CRC=RecieveBuf[recieve_count-1];

			    if(CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
				{
					PT_RESTART(pt);//если CRC не сошлось-перезапустим протокол	 
				}
			}
			break;
	
			case PROTO_TYPE_OLD:
			{
				CRC=RecieveBuf[recieve_count-1];

				if(Old_CRC_Check(&RecieveBuf,(recieve_count-CRC_LEN))!=CRC)
				{
					PT_RESTART(pt);//если CRC не сошлось-перезапустим протокол	 
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
					PT_RESTART(pt);//если LRC не сошлось-перезапустим протокол	 		 
				}
			}
			break;
		}

	//	PT_YIELD(pt);//дадим другим процессам время
  //-----------------------------
  		ProtoBufHandling();//процедура обработки сообщения	
		if(buf_len==0)//если в буфере пусто
		{
			PT_RESTART(pt);//перезапустим протокол	
		}
		else
		{
			buf_len=Proto_Paste_Null(TransferBuf,buf_len);
			DE_RE=1; //переключаем RS485 в режим передачи
							
			REN=0;	//запрет приема-только передача
			transf_count=0;
		//	CUT_OUT_NULL=0;
			SBUF=TransferBuf[transf_count];//передача байта, остальным займется автомат
			transf_count++;//инкрементируем счетчик переданных
			ES=1; //включим прерывание уарт	

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


 PT_THREAD(RingBufHandling(struct pt *pt))//обработка кольцевого буфера
 {
 	static unsigned char temp_count=0;//временные копии конца  и счетчика буфера
	static unsigned char temp_tail=0;
//	static unsigned char frame_count=0;//счетчик формируемого кадра	 протокола

	static unsigned char i=0;

  PT_BEGIN(pt);

  while(1) 
  {
  		PT_YIELD_UNTIL(pt,(rngbuf.count>0x0));//в буфере есть данные

		ES=0;
		temp_count=rngbuf.count;
		rngbuf.count=0x0;
		temp_tail =rngbuf.tail;
		ES=1;

		for(i=(temp_tail-temp_count);i!=temp_tail;i++)//поиск стартовой последовательности
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
		 				if(CUT_OUT_NULL==1)	  //если после 0xD7-пропускаем
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
			   		  if(recieve_count==6+frame_len)	  // принимаем указанное в frame_len число байт данные, 6 значит что обмен идет с компом, надо ставаить 5 чтобы обмениваться с устройствами
		   			  {
							RECIEVED=1;//буфер принят
					  		ES=0;
						//	rngbuf.tail=0;
					  		REN=0;  //recieve disable -запрещаем принимать в буфер	
						   	CUT_OUT_NULL=0;  			  			
					  }	 
					  
					  //----------------------обрабатываем возможные ошибки длины кадра-------------
				      if(recieve_count>MAX_LENGTH_REC_BUF)	//если посылка слишком длинная
					  {
							PT_RESTART(&pt_proto);  //внепроцессный рестарт
					  }  
			   }
			   else
			   {
					   if(recieve_count==6)
					   {     
				        	
							frame_len=RecieveBuf[recieve_count-1];  // получаем длину данных после заголовка
							if(protocol_type==PROTO_TYPE_OLD)
							{
								frame_len&=0x1F;//в старом протоколе только 5 младших бит -длина оставшейся части	
							}					 
					   }	   		
			   }														
		}	
  }

  PT_END(pt);
 }