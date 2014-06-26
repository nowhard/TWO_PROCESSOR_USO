#ifndef I2C_H
#define I2C_H
#include <ADuC845.h>
//#include "rtos/core.h"
#include "pt/pt.h"
#include<stdio.h>
#include "proto_uso/channels.h"

#define I2C_ADDR 0x58
#define I2C_FRAME_SIZE	12
//------------------------------------------
struct I2C_Channel	   //структура кадра I2C
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
			unsigned char protocol_type;
			unsigned char CRC; 
		} channels;
	
		unsigned char i2c_buf[I2C_FRAME_SIZE];//сериализованная часть структуры
	} I2C_CHNL;
} ;
//-------------------------------------------
enum
{
	SLAVE_STATE_BYTE_RESET=0x12,
	SLAVE_DOL_COUNT_RESET =0x13
};

enum
{
	PROTO_GEOSPHERE=0,
	PROTO_MB_ASCII=1
};


void I2C_Init(void); //инициализация I2C
	
PT_THREAD(I2C_Process(struct pt *pt));//главный циклический процесс шины;


void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len);//запрос с двумя стартами

//void I2C_Is_Write(void);//если запись прошла успешно
//void I2C_Is_Read(void);	//если чтеное прошло успешно


extern struct Channel xdata channels[CHANNEL_NUMBER];
#endif