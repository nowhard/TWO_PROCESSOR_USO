#ifndef I2C_H
#define I2C_H
#include <ADuC845.h>
//#include "rtos/core.h"
#include "pt/pt.h"
#include<stdio.h>
#include "proto_uso/channels.h"

#define I2C_ADDR 0x58
//------------------------------------------
struct I2C_Channel	   //��������� ����� I2C
{	
	union 
	{
		struct
		{
			unsigned long DOL;
			unsigned char frequency_modific;
			unsigned int frequency;
			unsigned int mid_frequency;
			unsigned char state_byte;
			unsigned char CRC; 
		} channels;
	
		unsigned char i2c_buf[11];//��������������� ����� ���������
	} I2C_CHNL;
} ;
//-------------------------------------------

void I2C_Init(void); //������������� I2C
	
PT_THREAD(I2C_Process(struct pt *pt));//������� ����������� ������� ����;


void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len);//������ � ����� ��������

//void I2C_Is_Write(void);//���� ������ ������ �������
//void I2C_Is_Read(void);	//���� ������ ������ �������


extern struct Channel xdata channels[CHANNEL_NUMBER];
#endif