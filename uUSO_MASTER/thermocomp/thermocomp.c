#include "thermocomp.h"
#pragma NOAREGS
//-------------------------------------------------------------------------
/*void FillTermTable(void)//���������� ������� ����������������
{
	
}
//-------------------------------------------------------------------------
void StoreTermTable(void)//���������� ������� ����������������	� EEPROM
{
 	EEPROM_Write(&TermTable,TABLE_LEN,TABLE_EEPROM_ADDR);
}
//-------------------------------------------------------------------------
void RestoreTermTable(void)//�������� ������� ���������������� �� EEPROM
{
	EEPROM_Read(&TermTable,TABLE_LEN,TABLE_EEPROM_ADDR);
}
//-------------------------------------------------------------------------
long TermCompensate(long temp,long pressure)//������� ��������� ����������������
{

return 0;
}	*/
//-------------------------------------------------------------------------
void SetFirstThmPoint(float temp, float	voltage) //��������� ������ ����� ����������������
{
		first_thm=temp;
		first_vlt=voltage;
		EEPROM_Write(&first_thm,1,FRST_PNT_THM_ADDR);
		EEPROM_Write(&first_vlt,1,FRST_PNT_VLT_ADDR);
}
//-------------------------------------------------------------------------
void SetSecondThmPoint(float temp, float voltage) //��������� ������ ����� ����������������
{
		second_thm=temp;
		second_vlt=voltage;
		EEPROM_Write(&second_thm,1,SCND_PNT_THM_ADDR);
		EEPROM_Write(&second_vlt,1,SCND_PNT_VLT_ADDR);
}
//-------------------------------------------------------------------------
void SetCompensate(unsigned char point)//��������� ����� �����������
{
	switch(point)
	{
		case 0x3:
		{
			SetFirstThmPoint(Get_Temperature(),Get_Pressure());
		}
		break;

		case 0x4:
		{
			SetSecondThmPoint(Get_Temperature(),Get_Pressure());
		}
		break;
	}

	thm_coeff=(second_vlt-first_vlt)/(second_thm-first_thm);
}
//-------------------------------------------------------------------------
float Compensate(float voltage,float temp)//������� ����������� 
{
	return voltage-(temp-first_thm)*thm_coeff;
}
//-------------------------------------------------------------------------
void RestoreCompensate(void)//�������������� �������������������� ������������ ����� ���������
{
		EEPROM_Read(&first_thm,1,FRST_PNT_THM_ADDR);
		EEPROM_Read(&first_vlt,1,FRST_PNT_VLT_ADDR);
		EEPROM_Read(&second_thm,1,SCND_PNT_THM_ADDR);
		EEPROM_Read(&second_vlt,1,SCND_PNT_VLT_ADDR);

		thm_coeff=(second_vlt-first_vlt)/(second_thm-first_thm);
}
//-------------------------------------------------------------------------