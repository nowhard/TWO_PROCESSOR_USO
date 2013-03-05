#include "i2c_slave.h"
//--------------------------------------------------------
volatile unsigned char i2c_slave_buf[0x20];//�������� ����� i2c
volatile unsigned char channel_num=0;
volatile unsigned char count=0;
//--------------------------------------------------------
void I2C_Slave_Init(void) //������������� ���� i2c slave
{
	I2CCON=0x00; // �������
	I2CADD=0xB0;
	IEIP2=0xA1; // ���������� ���������� �� I2C	
	return;
}
//--------------------------------------------------------
void I2C_ISR(void) interrupt 7//���������� I2C
{
	if(I2CTX==0)  //�����
	{ 
		channel_num=I2CDAT;
	}
	else   //��������
	{
		if(count<4)
		{
			I2CDAT=(unsigned char*)(&channels[channel_num].channel_data)[count];
			count++;
		}
		else
		{
			count=0;
			I2CRS=1;//����� ����������
		}
	}
	return;
}
//--------------------------------------------------------