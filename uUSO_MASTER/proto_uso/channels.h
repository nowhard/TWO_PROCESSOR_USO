#ifndef CHANNELS_H
#define CHANNELS_H
#include "eeprom/eeprom.h"
#include "preferences.h"
//---------------------------------------
#define CHANNEL_NUMBER	14//количество каналов
//---------------------------каналы---------------------------------
#define CHNL_ADC	0//канал ацп
	#define CHNL_ADC_FIX_16	0
	#define CHNL_ADC_SW_16	1
	#define CHNL_ADC_FIX_24	3
	#define CHNL_ADC_SW_24	2
#define CHNL_DOL	1 //канал дол
	#define CHNL_DOL_ENC	0
	#define CHNL_DOL_FLOAT	1
#define CHNL_FREQ	2//частотный/счетчик
	#define	 CHNL_FREQ_COUNT_T	0
	#define	 CHNL_FREQ_256		1
	#define  CHNL_FREQ_PERIOD	3
	#define	 CHNL_FREQ_COUNT	4
//------------------------------------------------------------------
struct Channel
{
	unsigned char number ;	  	// номер канала 
	
	union 
	{
		struct
		{
			unsigned char type;		  // тип канала
			unsigned char modific;	  // модификатор канала
			unsigned char state_byte_1;	// байт состояния канала
			unsigned char state_byte_2; 
		}set;
	
		unsigned long serialize;//сериализованная часть структуры
	} settings;
	
	unsigned long channel_data;		  // значение канала
	unsigned long channel_data_calibrate;//значение канала калиброванное

	union
	{
		struct
		{
			unsigned char calibrate;//флаг калиброванности канала
			float K;//
			float C;//y=kx+c		
		} cal;
		unsigned long serialize[3];//сериализованная часть структуры
	} calibrate;
	
} ;
//---------------------------------------
void ChannelsInit(void);//инициализация структуры каналов
void Store_Channels_Data(void);//Сохраним данные каналов в ППЗУ
void Restore_Channels_Data(void);//Восстановим данные каналов из ППЗУ
void Channels_Set_Default(void);//сбросить значения каналов по умолчанию
//---------------------------------------
#endif