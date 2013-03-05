#include "dac.h"
//-------------------------------------------------------------------------------
static float val_current;//������ �������� ����
//-------------------------------------------------------------------------------
void DAC_Init(void)//������������� ���
{
	float current=0;
	DACCON=0x3;//dac enable  Vref (AINCOM)
	current= Restore_DAC_Settings();
	if(current<=0 || current>=DAC_MAX_CURRENT)
	{
		DAC_Set_Current(0);//������������� �������� �� ����
	}
	else
	{
		DAC_Set_Current(current);//������������� �������� �� ����
	}
	return;
}
//-------------------------------------------------------------------------------
void DAC_Set_Voltage(float voltage) //���������� ���������� �� ���
{
	   unsigned int DAC_code=0x0;
	   DAC_code=(unsigned int)(voltage*(float)(0xFFF)/REF_VOLTAGE);
	 	 
	   DACH= (DAC_code>>8)&0xF;
	   DACL= DAC_code&0xFF;
}
//------------------------------------------------------------------------------
void DAC_Set_Current(float current)	//��������� ����
{
 	 val_current=current; 
	 DAC_Set_Voltage(current*RESISTOR/1000);
}
//-------------------------------------------------------------------------------
void Store_DAC_Settings(float voltage)//���������� ���������� ��� � ����
{ 	
	EEPROM_Write(&voltage,1,DAC_SETTINGS_ADDR);
	return;
}
//-------------------------------------------------------------------------------
float Restore_DAC_Settings(void) //������������ �������� ���������� �� ��� �� ����
{
	float dac_voltage=0;
	EEPROM_Read(&dac_voltage,1,DAC_SETTINGS_ADDR);
	return dac_voltage;	
}
//-------------------------------------------------------------------------------
unsigned long DAC_Get_Settings(void)//�������� ��������� ���
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