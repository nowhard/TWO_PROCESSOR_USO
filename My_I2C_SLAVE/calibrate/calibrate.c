#include "calibrate.h"
#pragma NOAREGS
//--------------------global variables------------------------
static long  first_point_x=0; //первая точка калибровки
static long  second_point_x=0;//вторая точка калибровки
static float first_point_y=0;//плотность первого калибровочного раствора
static float second_point_y=0;//плотность второго калибровочного раствора
//------------------------------------------------------------
void SetFirstPoint(long ADC_Code,float val)	 //установка первой точки 
{
		first_point_x=ADC_Code;
		first_point_y=val;
		EEPROM_Write(&ADC_Code,1,FIRST_POINT_X_ADRESS);
		EEPROM_Write(&val,1,FIRST_POINT_Y_ADRESS);
}
//------------------------------------------------------------
void SetSecondPoint(long ADC_Code,float val) //установка второй точки 
{
	    second_point_x=ADC_Code;
		second_point_y=val;
		EEPROM_Write(&ADC_Code,1,SECOND_POINT_X_ADRESS);
		EEPROM_Write(&val,1,SECOND_POINT_Y_ADRESS);
}
//------------------------------------------------------------
void Calibrate(unsigned char point,float val) //общая функция калибровки
{
	switch(point)
	{
		case 1:
		{
			SetFirstPoint(Get_ADC_Code(AIN1_AIN2),val);	
		}
		break;

		case 2:
		{
			SetSecondPoint(Get_ADC_Code(AIN1_AIN2),val);	
		}
		break;

		default:
		{
			return;
		}
	}
}
//------------------------------------------------------------
float GetCalibrateVal(long ADC_Code)   //преобразование значения АЦП в калиброванное значение	 ??? проверить && debug
{
  	float  val=0;//плотность
	val=(((float)(ADC_Code-first_point_x)/(float)(second_point_x-first_point_x))*(float)(second_point_y-first_point_y))+first_point_y; //формула калибровки по 2м точкам
	return val;
}
//------------------------------------------------------------
void RestoreCalibrate(void)		 //восстановление точек калибровки из EEPROM
{
	EEPROM_Read(&first_point_x,1,FIRST_POINT_X_ADRESS);	
	EEPROM_Read(&second_point_x,1,SECOND_POINT_X_ADRESS);

	EEPROM_Read(&first_point_y,1,FIRST_POINT_Y_ADRESS);	
	EEPROM_Read(&second_point_y,1,SECOND_POINT_Y_ADRESS);
}
//---------------------------------------------------------------
