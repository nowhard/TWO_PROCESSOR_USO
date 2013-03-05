#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "proto_uso/channels.h"
#include "adc.h"

//функции калибровки по двум точкам
//
//
//------------------------------------------------------------
/*#define FIRST_POINT_X_ADRESS 	0x1
#define FIRST_POINT_Y_ADRESS	0x2
#define SECOND_POINT_X_ADRESS	0x3
#define SECOND_POINT_Y_ADRESS	0x4*/


//------------------------------------------------------------
void SetFirstPoint(unsigned char channel_num,long ADC_Code,unsigned long val);	 //установка первой точки -воздух плотность = 0
void SetSecondPoint(unsigned char channel_num,long ADC_Code,unsigned long val); //установка второй точки -вода	  плотность = 1

//float GetFirstDensityPoint(void);
//float GetSecondDensityPoint(void);

void Calibrate(unsigned char channel_num,float K,float C); //общая функция калибровки
void RestoreCalibrate(void);		 //восстановление точек калибровки из EEPROM
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //преобразование значения АЦП в значение плотности в кг\м^3

//float GetCompensedDensity(float pressure,float temp);   //преобразование значения АЦП в значение плотности в кг\м^3
 
//------------------------------------------------------------
#endif