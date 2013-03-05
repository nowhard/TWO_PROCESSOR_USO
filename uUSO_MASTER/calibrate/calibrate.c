#include "calibrate.h"

//------------------------------------------------------------
/*void SetFirstPoint(unsigned char channel_num,long ADC_Code,unsigned long val)	 //установка первой точки 
{	  


		channels[channel_num].calibrate.cal.first_point_x=ADC_Code;
		channels[channel_num].calibrate.cal.first_point_y=val;
	//	channels[channel_num].calibrate.cal.calibrate=1;

		channels[channel_num].calibrate.cal.K= (float)(channels[channel_num].calibrate.cal.second_point_y-channels[channel_num].calibrate.cal.first_point_y)/(float)(channels[channel_num].calibrate.cal.second_point_x-channels[channel_num].calibrate.cal.first_point_x);
		channels[channel_num].calibrate.cal.C= (float)channels[channel_num].calibrate.cal.first_point_y-(float)channels[channel_num].calibrate.cal.first_point_x*channels[channel_num].calibrate.cal.K;

	//	EEPROM_Write(&channels[channel_num].calibrate.serialize,7,ADC_CALIBRATE_ADDR+channel_num*7);
	
		return;
}*/
//------------------------------------------------------------
/*void SetSecondPoint(unsigned char channel_num,long ADC_Code,unsigned long val) //установка второй точки 
{		

		channels[channel_num].calibrate.cal.second_point_x=ADC_Code;
		channels[channel_num].calibrate.cal.second_point_y=val;

		channels[channel_num].calibrate.cal.K= (float)(channels[channel_num].calibrate.cal.second_point_y-channels[channel_num].calibrate.cal.first_point_y)/(float)(channels[channel_num].calibrate.cal.second_point_x-channels[channel_num].calibrate.cal.first_point_x);
		channels[channel_num].calibrate.cal.C= (float)channels[channel_num].calibrate.cal.first_point_y-(float)channels[channel_num].calibrate.cal.first_point_x*channels[channel_num].calibrate.cal.K;	

	//	EEPROM_Write(&channels[channel_num].calibrate.serialize,7,ADC_CALIBRATE_ADDR+channel_num*7);


		return;
} */
//------------------------------------------------------------
unsigned long GetCalibrateVal(unsigned char channel_num,unsigned long ADC_Code)   //преобразование значени€ ј÷ѕ в калиброванное значение	 ??? проверить && debug
{
  //	unsigned long  val=0;//
//	LED=1;
	if(ADC_Code==0)
		return 0;
//	return 	(unsigned long)((((float)(ADC_Code-channels[channel_num].calibrate.cal.first_point_x)/(channels[channel_num].calibrate.cal.second_point_x-channels[channel_num].calibrate.cal.first_point_x))*(channels[channel_num].calibrate.cal.second_point_y-channels[channel_num].calibrate.cal.first_point_y)))+channels[channel_num].calibrate.cal.first_point_y; //формула калибровки по 2м точкам
	return (unsigned long)(ADC_Code*channels[channel_num].calibrate.cal.K+channels[channel_num].calibrate.cal.C);
//	LED=0;
	
}
//------------------------------------------------------------
void RestoreCalibrate(void)		 //восстановление точек калибровки из EEPROM
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].calibrate.serialize,3,ADC_CALIBRATE_ADDR+i*3);
	}

	return;
}
//---------------------------------------------------------------
void Calibrate(unsigned char channel_num,float K,float C) //обща€ функци€ калибровки
{
		channels[channel_num].calibrate.cal.K= K;
		channels[channel_num].calibrate.cal.C= C;
		EEPROM_Write(&channels[channel_num].calibrate.serialize,3,ADC_CALIBRATE_ADDR+channel_num*3);
		return;
}
//---------------------------------------------------------------