#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "adc.h"
#include "thermocomp/thermocomp.h"
//������� ���������� �� ���� ������
//
//
//------------------------------------------------------------
#define FIRST_POINT_X_ADRESS 	0x1
#define FIRST_POINT_Y_ADRESS	0x2
#define SECOND_POINT_X_ADRESS	0x3
#define SECOND_POINT_Y_ADRESS	0x4


//------------------------------------------------------------
static void SetFirstPoint(long ADC_Code,float val);	 //��������� ������ ����� -������ ��������� = 0
static void SetSecondPoint(long ADC_Code,float val); //��������� ������ ����� -����	  ��������� = 1

//float GetFirstDensityPoint(void);
//float GetSecondDensityPoint(void);

void Calibrate(unsigned char point,float val); //����� ������� ����������
void RestoreCalibrate(void);		 //�������������� ����� ���������� �� EEPROM
float GetCalibrateVal(long ADC_Code);   //�������������� �������� ��� � �������� ��������� � ��\�^3

//float GetCompensedDensity(float pressure,float temp);   //�������������� �������� ��� � �������� ��������� � ��\�^3

//------------------------------------------------------------
#endif