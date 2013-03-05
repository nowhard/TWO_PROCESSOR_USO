#include "dol.h"

#define DOL_CHANNELS	1
#define FIRST_DOL_CHN	0

#define CLI_EXT  EX0=0;
#define STI_EXT  EX0=1; 

sbit INTINP=P3^2;//����� ����������
sbit DIRECT=P3^3 ;
sbit INVERT=P3^4 ;

//sbit LED=P0^6;
//-------------------------------------------------------- debug
 long volatile idata counter_32; //������� ����


//--------------------------------------------------------
void Dol_Init(void) //�������������
{
	counter_32=0x80008000;
	//channels.I2C_CHNL.channels.DOL=counter_32;
	channels.DOL_buf=counter_32;
	DIRECT=1;
	INVERT=0;
	IT0=1;//�� ������
	IE0=0;
	EX0=1;
	PX0=1;//��������� �������
	if(!INTINP)	//���� ������ � ������ ������-�������� �������� ������� ��������, �����������
		INVERT=!INVERT;

//	LED=0;
	return;
}
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

	if(channels.transfer==0)	//���� �������� ���
	{
		//channels.I2C_CHNL.channels.DOL=counter_32;
		channels.DOL_buf=counter_32;
	}
	
	STI_EXT
	return;
}
//------------------------------------------------------------
