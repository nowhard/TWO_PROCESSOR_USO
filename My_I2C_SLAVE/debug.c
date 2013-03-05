#include <ADuC845.h>

#include "timer2.h"

#include "preferences.h"
#include "rtos/timer1.h"
#include "watchdog.h"

#include "dol.h"
#include "i2c_slave.h"

 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram
	
	PLLCON&=PLLCON_VAL;//��������� ������� ����������
//	ChannelsInit();//������������� �������� �������

	
	Timer1_Initialize(); //
	Timer2_Initialize();

	Dol_Init();

	WDT_Init(WDT_2000);//�������� ���������� ������

	I2C_Slave_Init();
	EA=1;

	while(1)
	{		
	  Frequency_Handler();//���������� ��������� �������
	  WDT_Clear();
	}
}
//-----------------------------------------------------------------------------
