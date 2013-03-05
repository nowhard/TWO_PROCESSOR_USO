#ifndef CALIBRATE_H
#define CALIBRATE_H
#include "eeprom/eeprom.h"
#include "adc.h"
#include "thermocomp/thermocomp.h"
//функции калибровки по двум точкам
//
//
//------------------------------------------------------------
#define FIRST_POINT_X_ADRESS 	0x1
#define FIRST_POINT_Y_ADRESS	0x2
#define SECOND_POINT_X_ADRESS	0x3
#define SECOND_POINT_Y_ADRESS	0x4


//------------------------------------------------------------
static void SetFirstPoint(long ADC_Code,float val);	 //установка первой точки -воздух плотность = 0
static void SetSecondPoint(long ADC_Code,float val); //установка второй точки -вода	  плотность = 1

//float GetFirstDensityPoint(void);
//float GetSecondDensityPoint(void);

void Calibrate(unsigned char point,float val); //общая функция калибровки
void RestoreCalibrate(void);		 //восстановление точек калибровки из EEPROM
float GetCalibrateVal(long ADC_Code);   //преобразование значения АЦП в значение плотности в кг\м^3

//float GetCompensedDensity(float pressure,float temp);   //преобразование значения АЦП в значение плотности в кг\м^3

//------------------------------------------------------------
#endif