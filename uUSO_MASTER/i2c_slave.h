#ifndef I2C_SLAVE_H
#define	I2C_SLAVE_H
#include <ADuC845.h>
#include "proto_uso/channels.h"
//-----------------------------------
extern struct Channel xdata channels[CHANNEL_NUMBER];
//--------------������� I2C---------
void I2C_Slave_Init(void);//������������� ���� i2c slave
void I2C_ISR(void);//���������� I2C
//void I2C_
//----------------------------------

#endif