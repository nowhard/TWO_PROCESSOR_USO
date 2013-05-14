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
void Calibrate(unsigned char channel_num,float K,float C); //общая функция калибровки
void RestoreCalibrate(void);		 //восстановление точек калибровки из EEPROM
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code);   //преобразование значения АЦП в значение плотности в кг\м^3
unsigned char Calibrate_Get_CRC(void);//расчет crc  в eerprom 
void Calibrate_Set_Flag(unsigned char channel,unsigned char flag);//установить/снять флаг калиброванности канала
void Calibrate_Set_Default(void);//установка калибровки по умолчанию(отключена)
//------------------------------------------------------------
#endif