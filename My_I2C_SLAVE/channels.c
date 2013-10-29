#include "channels.h"


//-----------------------------------
volatile struct Channel xdata channels; 
//-----------------------------------
/*void ChannelsInit(void) //using 0//инициализация структуры каналов
{
	
	Restore_Channels_Data();
	if(channels[0].settings.set.type!=1 || channels[0].settings.set.modific!=0)
	{	
		// КАНАЛ 0 - ДОЛ I2C
		channels[0].number=0;		 
		channels[0].settings.set.type=1;
		channels[0].settings.set.modific=0;
		channels[0].settings.set.state_byte_1=0x40;
		channels[0].settings.set.state_byte_2=0x0A;
		channels[0].channel_data=0x0;

				// КАНАЛ 1 частота
		channels[1].number=1;		 
		channels[1].settings.set.type=2;
		channels[1].settings.set.modific=0;
		channels[1].settings.set.state_byte_1=0x40;
		channels[1].settings.set.state_byte_2=0x0A;
		channels[1].channel_data=0x0;

				// КАНАЛ 2 частота средняя
		channels[2].number=2;		 
		channels[2].settings.set.type=2;
		channels[2].settings.set.modific=0;
		channels[2].settings.set.state_byte_1=0x40;
		channels[2].settings.set.state_byte_2=0x0A;
		channels[2].channel_data=0x0;
	}
	return;
}
//-----------------------------------
void Store_Channels_Data(void) //using 0//Сохраним данные каналов в ППЗУ
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Write(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}
	return;
}
//-----------------------------------
void Restore_Channels_Data(void) //using 0//Восстановим данные каналов из ППЗУ
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}
	return;
}	*/
