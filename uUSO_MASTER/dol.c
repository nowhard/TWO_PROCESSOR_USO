#include "dol.h"

#define DOL_CHANNELS	1
#define FIRST_DOL_CHN	8

#define CLI_EXT  EX0=0;
#define STI_EXT  EX0=1; 

sbit INTINP=P3^2;//����� ����������
sbit DIRECT=P3^3 ;
sbit INVERT=P3^4 ;

sbit LED=P0^6;
//-------------------------------------------------------- debug
 long volatile idata counter_32; //������� ����

#define TIMER_FREE 0  //������ �������� ��� �������
#define BUSY_CHN1  1
#define BUSY_CHN2  2
#define BUSY_CHN3  3

#define FRQ_CHNL_1 0
#define FRQ_CHNL_2 1
#define FRQ_CHNL_3 2
//--------------------------------------------------------
void Dol_Init(void) //������������� ��������� �������
{
	counter_32=0x80008000;
	channels[FIRST_DOL_CHN].channel_data=counter_32;
	DIRECT=1;
	INVERT=0;
	IT0=1;//�� ������
	IE0=0;
	EX0=1;
	
	if(!INTINP)	//���� ������ � ������ ������-�������� �������� ������� ��������, �����������
		INVERT=!INVERT;

	LED=0;
	return;
}
//--------------------------------------------------------
/*unsigned char Dol_Measure_Process(void) //using 2//����������� ������� ��������� 
{
	channels[FIRST_DOL_CHN].channel_data=counter_32;
	LED=!LED;	
	setTimer(DOL_TASK, 0, 50);
	return 0;
}*/
//--------------------------------------------------------
void INT0_ISR(void) interrupt 0 //using 3//���������� �������� ���������� 0
{
	CLI_EXT
	INVERT=!INVERT;
	if(DIRECT)
	{
		counter_32++;
	}
	else
	{
		counter_32--;
	}
	channels[FIRST_DOL_CHN].channel_data=counter_32;
	STI_EXT
	return;
}
//------------------------------------------------------------
unsigned char Sec_Task(void) //using 2//��������� ������ 
{	

	WDT_Clear();
	//--------------------
	setTimer(SEC_TASK, 0, 200);
	return 0;
}