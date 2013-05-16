#include "calibrate.h"
//#pragma OT(6,Speed)
//------------------------------------------------------------
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code)   //�������������� �������� ��� � ������������� ��������	 ??? ��������� && debug
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
void RestoreCalibrate(void)		 //�������������� ����� ���������� �� EEPROM
{
	unsigned char i=0;
	unsigned long crc=0,true_crc=0;

	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].calibrate.serialize[0],3,ADC_CALIBRATE_ADDR+i*3);
	}

	true_crc= (unsigned long)Calibrate_Get_CRC();//������ ������� CRC ����������
	EEPROM_Read(&crc,1,CALIBRATE_DEVICE_CRC_ADDR);//������� CRC ��������

	if(true_crc!=crc)
	{
		Calibrate_Set_Default();
	}

	return;
}
//---------------------------------------------------------------
void Calibrate(unsigned char channel_num,float K,float C) //����� ������� ����������
{
		unsigned long true_crc=0;
		channels[channel_num].calibrate.cal.K= K;
		channels[channel_num].calibrate.cal.C= C;
		EEPROM_Write(&channels[channel_num].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel_num*3);

		true_crc= (unsigned long)Calibrate_Get_CRC();//������ ������� CRC ����������
		EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//�������� CRC 	

		return;
}
//---------------------------------------------------------------
unsigned char Calibrate_Get_CRC(void)//������ crc  � eerprom
{
	 unsigned char crc = 0x0;
	 unsigned char i=0,j=0;

     for(i=0;i<CHANNEL_NUMBER;i++)
	 {
		for(j=0;j<(3*sizeof(unsigned long));j++)
		{
			crc = Crc8Table[crc ^ (((unsigned char *)(&channels[i].calibrate.serialize))[j])];
		}
	 }

     return crc;
}
//---------------------------------------------------------------
void Calibrate_Set_Flag(unsigned char channel,unsigned char flag)//����������/����� ���� ��������������� ������
{
    unsigned long true_crc=0;

	if(flag==SET)
	{
			channels[channel].calibrate.cal.calibrate=1;//���������/������ ���� ����������
			EEPROM_Write(&channels[channel].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel*3);		
	}
	else
	{
		if(flag==RESET)
		{
			channels[channel].calibrate.cal.calibrate=0;//���������/������ ���� ����������
			EEPROM_Write(&channels[channel].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel*3);
		}
	}

  	true_crc= (unsigned long)Calibrate_Get_CRC();//������ ������� CRC ����������
	EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//�������� CRC 	
}
//----------------------------------------------------------------
void Calibrate_Set_Default(void)//��������� ���������� �� ���������(���������)
{
		unsigned char i=0;
	    unsigned long true_crc=0;		

		for(i=0;i<CHANNEL_NUMBER;i++)
		{
			Calibrate_Set_Flag(i,RESET);
		}
		
		true_crc= (unsigned long)Calibrate_Get_CRC();//������ ������� CRC ����������
		EEPROM_Write(&true_crc,1,CALIBRATE_DEVICE_CRC_ADDR);//�������� CRC 		
}
//----------------------------------------------------------------