#include <ADuC845.h>


#include "preferences.h"
#include "watchdog.h"

#include "dol.h"
#include "i2c_slave.h"
#include "frequency.h"

extern struct Channel xdata channels;//������ �� ���������� I2C

sbit FREQ_IN_CONF=P2^0;
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram
	
	PLLCON&=PLLCON_VAL;//��������� ������� ����������
//	ChannelsInit();//������������� �������� �������
	FREQ_IN_CONF=1;//�������� �� 1 ������� ��������� ���������� �����

	if(FREQ_IN_CONF==0)//������� ����������
	{
		channels.I2C_CHNL.channels.frequency_modific=CHNL_FREQ_256;	
	}
	else
	{
		channels.I2C_CHNL.channels.frequency_modific=CHNL_FREQ_COUNT_T;
	}

	Frequency_Init(channels.I2C_CHNL.channels.frequency_modific);

	Dol_Init();

	WDT_Init(WDT_2000);//�������� ���������� ������

	I2C_Slave_Init();
	EA=1;

	while(1)
	{		
	  Frequency_Measure_Process();
	  WDT_Clear();
	}
}
//-----------------------------------------------------------------------------
