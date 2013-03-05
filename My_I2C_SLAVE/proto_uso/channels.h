#ifndef CHANNELS_H
#define CHANNELS_H
#include "eeprom/eeprom.h"
#include "preferences.h"
//---------------------------------------
//#define CHANNEL_NUMBER	3//количество каналов
//---------------------------------------
struct Channel
{	
	union 
	{
		struct
		{
			unsigned long DOL;
			unsigned int frequency;
			unsigned int mid_frequency;
			unsigned char state_byte;
			unsigned char CRC; 
		} channels;
	
		unsigned char i2c_buf[10];//сериализованная часть структуры
	} I2C_CHNL;
	unsigned char transfer;
	unsigned long DOL_buf;		  //буферные переменные
	unsigned int frequency_buf;
	unsigned int mid_frequency_buf;

} ;
//---------------------------------------
//void ChannelsInit(void);//инициализация структуры каналов
//void Store_Channels_Data(void);//Сохраним данные каналов в ППЗУ
//void Restore_Channels_Data(void);//Восстановим данные каналов из ППЗУ

//---------------------------------------
#endif