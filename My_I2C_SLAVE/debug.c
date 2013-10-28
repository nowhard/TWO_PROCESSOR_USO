#include <ADuC845.h>

#include "timer2.h"

#include "preferences.h"
#include "rtos/timer1.h"
#include "watchdog.h"

#include "dol.h"
#include "i2c_slave.h"

extern struct Channel xdata channels;//каналы по интерфейсу I2C

sbit FREQ_IN_CONF=P2^0;
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram
	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
//	ChannelsInit();//инициализация настроек каналов
	FREQ_IN_CONF=1;//подтянем на 1 джампер настройки частотного входа

	if(FREQ_IN_CONF==0)//джампер установлен
	{
		channels.I2C_CHNL.channels.frequency_modific=CHNL_FREQ_256;	
	}
	else
	{
		channels.I2C_CHNL.channels.frequency_modific=CHNL_FREQ_COUNT_T;
	}

	
	Timer1_Initialize(); //
	Timer2_Initialize();

	Dol_Init();

	WDT_Init(WDT_2000);//включить сторожевой таймер

	I2C_Slave_Init();
	EA=1;

	while(1)
	{		
	  Frequency_Handler();//обработчик измерения частоты
	  WDT_Clear();
	}
}
//-----------------------------------------------------------------------------
