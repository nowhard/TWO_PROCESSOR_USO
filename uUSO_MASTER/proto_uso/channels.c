#include "channels.h"


//-----------------------------------
volatile struct Channel xdata channels[CHANNEL_NUMBER] /*_at_ 0x20*/;
//-----------------------------------
void ChannelsInit(void) //using 0//инициализация структуры каналов
{
	Restore_Channels_Data();
 //-----------------------------------------------------------------------
	if(channels[0].settings.set.type!=0 || channels[1].settings.set.type!=0)
	{	
		channels[0].settings.set.modific=CHNL_ADC_FIX_16;
		channels[0].settings.set.state_byte_1=0x40;
		channels[0].settings.set.state_byte_2=0x1E;
		channels[0].channel_data_calibrate=0;
		channels[0].calibrate.cal.calibrate=0;
		// КАНАЛ 2 - фиксированый АЦП
		channels[1].settings.set.modific=CHNL_ADC_FIX_16;
		channels[1].settings.set.state_byte_1=0x40;
		channels[1].settings.set.state_byte_2=0x1E;
		channels[1].channel_data_calibrate=0;
		channels[1].calibrate.cal.calibrate=0;
		// КАНАЛ 3 - фиксированый АЦП
		channels[2].settings.set.modific=CHNL_ADC_FIX_16;
		channels[2].settings.set.state_byte_1=0x40;
		channels[2].settings.set.state_byte_2=0x1E;
		channels[2].channel_data_calibrate=0;
		channels[2].calibrate.cal.calibrate=0;
		// КАНАЛ 4 - фиксированый АЦП
		channels[3].settings.set.modific=CHNL_ADC_FIX_16;
		channels[3].settings.set.state_byte_1=0x40;
		channels[3].settings.set.state_byte_2=0x1E;
		channels[3].channel_data_calibrate=0;
		channels[3].calibrate.cal.calibrate=0;
		// КАНАЛ 5 - фиксированый АЦП
		channels[4].settings.set.modific=CHNL_ADC_FIX_16;
		channels[4].settings.set.state_byte_1=0x40;
		channels[4].settings.set.state_byte_2=0x1E;
		channels[4].channel_data_calibrate=0;
		channels[4].calibrate.cal.calibrate=0;
		// КАНАЛ 6 - фиксированый АЦП
		channels[5].settings.set.modific=CHNL_ADC_FIX_16;
		channels[5].settings.set.state_byte_1=0x40;
		channels[5].settings.set.state_byte_2=0x1E;
		channels[5].channel_data_calibrate=0;
		channels[5].calibrate.cal.calibrate=0;
		// КАНАЛ 7 - фиксированый АЦП
		channels[6].settings.set.modific=CHNL_ADC_FIX_16;
		channels[6].settings.set.state_byte_1=0x40;
		channels[6].settings.set.state_byte_2=0x1E;
		channels[6].channel_data_calibrate=0;
		channels[6].calibrate.cal.calibrate=0;
		// КАНАЛ 8 - фиксированый АЦП
		channels[7].settings.set.modific=CHNL_ADC_FIX_16;
		channels[7].settings.set.state_byte_1=0x40;
		channels[7].settings.set.state_byte_2=0x1E;
		channels[7].channel_data_calibrate=0;
		channels[7].calibrate.cal.calibrate=0;
		// КАНАЛ 9 - Частотомер 0-256 Гц
		channels[8].settings.set.state_byte_1=0x40;
		channels[8].settings.set.state_byte_2=0x0A;
		channels[8].calibrate.cal.calibrate=0;
		// КАНАЛ 10 - Частотомер 0-256 Гц
		channels[9].settings.set.state_byte_1=0x40;
		channels[9].settings.set.state_byte_2=0x0A;
		channels[9].calibrate.cal.calibrate=0;
		// КАНАЛ 11 - Частотомер 0-256 Гц
		channels[10].settings.set.state_byte_1=0x40;
		channels[10].settings.set.state_byte_2=0x0A;
		channels[10].calibrate.cal.calibrate=0;
		// КАНАЛ 12 - ДОЛ I2C
		channels[11].settings.set.state_byte_1=0x40;
		channels[11].settings.set.state_byte_2=0x0A;
		channels[11].calibrate.cal.calibrate=0;
				// КАНАЛ 13 частота
		channels[12].settings.set.state_byte_1=0x40;
		channels[12].settings.set.state_byte_2=0x0A;
				// КАНАЛ 14 частота средняя
		channels[13].settings.set.state_byte_1=0x40;
		channels[13].settings.set.state_byte_2=0x0A;
	}
 		
		
		channels[0].number=0;		  
		channels[0].settings.set.type=CHNL_ADC;	
		channels[0].channel_data=0;
		// КАНАЛ 2 - фиксированый АЦП
		channels[1].number=0;
		channels[1].settings.set.type=CHNL_ADC;
		channels[1].channel_data=0;
		// КАНАЛ 3 - фиксированый АЦП
		channels[2].number=0;		 
		channels[2].settings.set.type=CHNL_ADC;
		channels[2].channel_data=0;
		// КАНАЛ 4 - фиксированый АЦП
		channels[3].number=0;		 
		channels[3].settings.set.type=CHNL_ADC;
		channels[3].channel_data=0;
		// КАНАЛ 5 - фиксированый АЦП
		channels[4].number=0xFF;		 
		channels[4].settings.set.type=CHNL_ADC;
		channels[4].channel_data=0;
		// КАНАЛ 6 - фиксированый АЦП
		channels[5].number=0xFF;		 
		channels[5].settings.set.type=CHNL_ADC;
		channels[5].channel_data=0;
		// КАНАЛ 7 - фиксированый АЦП
		channels[6].number=0xFF;		 
		channels[6].settings.set.type=CHNL_ADC;
		channels[6].channel_data=0;
		// КАНАЛ 8 - фиксированый АЦП
		channels[7].number=0xFF;		 
		channels[7].settings.set.type=CHNL_ADC;
		channels[7].channel_data=0;
		// КАНАЛ 9 - Частотомер 0-256 Гц
		channels[8].number=0xFF;		 
		channels[8].settings.set.type=CHNL_FREQ;
		channels[8].settings.set.modific=CHNL_FREQ_256;
		channels[8].channel_data=0x0;
		// КАНАЛ 10 - Частотомер 0-256 Гц
		channels[9].number=0xFF;		 
		channels[9].settings.set.type=CHNL_FREQ;
		channels[9].settings.set.modific=CHNL_FREQ_256;
		channels[9].channel_data=0x0;
		// КАНАЛ 11 - Частотомер 0-256 Гц
		channels[10].number=0xFF;		 
		channels[10].settings.set.type=CHNL_FREQ;
		channels[10].settings.set.modific=CHNL_FREQ_256;
		channels[10].channel_data=0x0;
		// КАНАЛ 12 - ДОЛ I2C
		channels[11].number=0xFF;		 
		channels[11].settings.set.type=CHNL_DOL;
		channels[11].settings.set.modific=CHNL_DOL_ENC;
		channels[11].channel_data=0x80008000;;
				// КАНАЛ 13 частота
		channels[12].number=0;		 
		channels[12].settings.set.type=CHNL_FREQ;
		channels[12].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[12].channel_data=0x0;
				// КАНАЛ 14 частота средняя
		channels[13].number=0;		 
		channels[13].settings.set.type=CHNL_FREQ;
		channels[13].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[13].channel_data=0x0;

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
}
