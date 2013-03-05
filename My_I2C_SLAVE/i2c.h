#ifndef I2C_H
#define I2C_H
#include <ADuC845.h>
#include "rtos/core.h"
#include<stdio.h>
#include "proto_uso/channels.h"
//------------I2C STATES--------------------
#define I2C_START			1
#define I2C_STOP			2
#define I2C_SET_ACK			3
#define I2C_SET_NACK		4
#define I2C_REPEAT_START	5
#define I2C_TRANSFER_BYTE	6
#define I2C_RECIEVE_BYTE	7
#define	I2C_DELAY			8
#define I2C_GET_ACK			9
#define I2C_WAIT_FOR_IO		10
#define I2C_SET_ADDR		11
#define I2C_RESTART			12
//------------------------------------------


struct I2C_STATE //��������� � ������������ I2C
{
	unsigned char GLOBAL_STATE;
	unsigned char LAST_GLOBAL_STATE;

	unsigned char START_STATE;
	unsigned char STOP_STATE;
	unsigned char RESTART_STATE;
	unsigned char SET_ACK_STATE;
	unsigned char GET_ACK_STATE;
	unsigned char SET_NACK_STATE;
	//unsigned char SET_ADDR_STATE;
	unsigned char TRANSFER_BYTE_STATE;
	unsigned char RECIEVE_BYTE_STATE;
	//unsigned char RECIEVE_BUF_STATE;
};

void I2C_Init(void); //������������� I2C
void I2C_Process(void);	//������� ����������� ������� ����
void I2C_Write_Buf(unsigned char addr,unsigned char *buf,unsigned char buf_len); //������ ������ � ����
void I2C_Read_Buf(unsigned char addr,unsigned char *buf,unsigned char buf_len);  //������ �� ���� � �����

void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len);//������ � ����� ��������

void I2C_Is_Write(void);//���� ������ ������ �������
void I2C_Is_Read(void);	//���� ������ ������ �������


extern struct Channel xdata channels;
#endif