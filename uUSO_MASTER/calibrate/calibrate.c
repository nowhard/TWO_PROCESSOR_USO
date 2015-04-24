#include "calibrate.h"
#pragma OT(6,Speed)
//------------------------------------------------------------
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code)   //преобразование значения АЦП в калиброванное значение	 ??? проверить && debug
{
	float result=0;

	result=ADC_Code*channels[channel_num].calibrate.cal.K+channels[channel_num].calibrate.cal.C;

	if(result>(float)0xFFFFFF)
	{
		return 0xFFFFFF;
	}
	else
	{
		if(result<0)
		{
			return 0;
		}
		else
		{
			return (unsigned long)result; 
		}
	}
}
//------------------------------------------------------------
void RestoreCalibrate(void)		 //восстановление точек калибровки из EEPROM
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].calibrate.serialize,3,ADC_CALIBRATE_ADDR+i*3);
	}

	return;
}
//---------------------------------------------------------------
void Calibrate(unsigned char channel_num,float K,float C) //общая функция калибровки
{
		channels[channel_num].calibrate.cal.K= K;
		channels[channel_num].calibrate.cal.C= C;
		EEPROM_Write(&channels[channel_num].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel_num*3);
		return;
}
//---------------------------------------------------------------