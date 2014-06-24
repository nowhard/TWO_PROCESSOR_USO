#ifndef I2C_SLAVE_H
#define	I2C_SLAVE_H
#include <ADuC845.h>
#include "channels.h"
//-----------------------------------
extern struct Channel xdata channels;

enum
{
	SLAVE_STATE_BYTE_RESET=0x12,
	SLAVE_DOL_COUNT_RESET =0x13
};
//--------------������� I2C---------
void I2C_Slave_Init(void);//������������� ���� i2c slave
void I2C_ISR(void);//���������� I2C
void I2C_Frame(void); //����������� ������� ������������ �����
static unsigned char  CRC_Check( unsigned char xdata *Spool_pr,unsigned char Count_pr );
//----------------------------------

#endif