#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "proto_uso/channels.h"
#include "adc.h"

//������� ���������� �� ���� ������
//
//
//------------------------------------------------------------
/*#define FIRST_POINT_X_ADRESS 	0x1
#define FIRST_POINT_Y_ADRESS	0x2
#define SECOND_POINT_X_ADRESS	0x3
#define SECOND_POINT_Y_ADRESS	0x4*/


//------------------------------------------------------------
void SetFirstPoint(unsigned char channel_num,long ADC_Code,unsigned long val);	 //��������� ������ ����� -������ ��������� = 0
void SetSecondPoint(unsigned char channel_num,long ADC_Code,unsigned long val); //��������� ������ ����� -����	  ��������� = 1

//float GetFirstDensityPoint(void);
//float GetSecondDensityPoint(void);

void Calibrate(unsigned char channel_num,float K,float C); //����� ������� ����������
void RestoreCalibrate(void);		 //�������������� ����� ���������� �� EEPROM
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //�������������� �������� ��� � �������� ��������� � ��\�^3

//float GetCompensedDensity(float pressure,float temp);   //�������������� �������� ��� � �������� ��������� � ��\�^3
 
//------------------------------------------------------------
#endif