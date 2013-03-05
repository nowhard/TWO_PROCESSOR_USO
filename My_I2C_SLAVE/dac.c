#include "dac.h"
//-------------------------------------------------------------------------------
static float val_current;//хранит значение тока
//-------------------------------------------------------------------------------
void DAC_Init(void)//инициализация ЦАП
{
	float current=0;
	DACCON=0x3;//dac enable  Vref (AINCOM)
	current= Restore_DAC_Settings();
	if(current<=0 || current>=DAC_MAX_CURRENT)
	{
		DAC_Set_Current(0);//восстанвление настроек из ППЗУ
	}
	else
	{
		DAC_Set_Current(current);//восстанвление настроек из ППЗУ
	}
	return;
}
//-------------------------------------------------------------------------------
void DAC_Set_Voltage(float voltage) //установить напряжение на ЦАП
{
	   unsigned int DAC_code=0x0;
	   DAC_code=(unsigned int)(voltage*(float)(0xFFF)/REF_VOLTAGE);
	 	 
	   DACH= (DAC_code>>8)&0xF;
	   DACL= DAC_code&0xFF;
}
//------------------------------------------------------------------------------
void DAC_Set_Current(float current)	//установка тока
{
 	 val_current=current; 
	 DAC_Set_Voltage(current*RESISTOR/1000);
}
//-------------------------------------------------------------------------------
void Store_DAC_Settings(float voltage)//сохранение напряжения ЦАП в ППЗУ
{ 	
	EEPROM_Write(&voltage,1,DAC_SETTINGS_ADDR);
	return;
}
//-------------------------------------------------------------------------------
float Restore_DAC_Settings(void) //восстановить значение напряжения на ЦАП из ППЗУ
{
	float dac_voltage=0;
	EEPROM_Read(&dac_voltage,1,DAC_SETTINGS_ADDR);
	return dac_voltage;	
}
//-------------------------------------------------------------------------------
unsigned long DAC_Get_Settings(void)//получить настройки цап
{
	union
	{
		float float_val;
		unsigned long long_val;
	}float_to_long;

	float_to_long.float_val=val_current;
	return float_to_long.long_val;	
}	
//-------------------------------------------------------------------------------