#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "proto_uso/channels.h"
#include "adc.h"
#include "crc_table.h"

enum 
{
	RESET=0,
	SET=1
} ;


//------------------------------------------------------------
void Calibrate(unsigned char channel_num,float K,float C); //����� ������� ����������
void RestoreCalibrate(void);		 //�������������� ����� ���������� �� EEPROM
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //�������������� �������� ��� � �������� ��������� � ��\�^3
unsigned char Calibrate_Get_CRC(void);//������ crc  � eerprom 
void Calibrate_Set_Flag(unsigned char channel,unsigned char flag);//����������/����� ���� ��������������� ������
void Calibrate_Set_Default(void);//��������� ���������� �� ���������(���������)
//------------------------------------------------------------
#endif