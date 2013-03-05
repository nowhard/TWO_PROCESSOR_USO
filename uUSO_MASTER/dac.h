#ifndef DAC_H
#define DAC_H
#include <ADuC845.h>
#include "preferences.h"
#include "eeprom/eeprom.h"
//------------------------------------------------------------
#define REF_VOLTAGE 2.5
#define DAC_MAX_CURRENT 15 //максимальный ток на уровне 15 мА
//------------------------------------------------------------
void DAC_Init(void);//инициализация ЦАП
void DAC_Set_Voltage(float voltage); //установить напряжение на ЦАП
void DAC_Set_Current(float current);	//установка тока
void Store_DAC_Settings(float voltage);//сохранение напряжения ЦАП в ППЗУ
float Restore_DAC_Settings(void); //восстановить значение напряжения на ЦАП из ППЗУ
unsigned long DAC_Get_Settings(void);//получить настройки цап
//------------------------------------------------------------


#endif