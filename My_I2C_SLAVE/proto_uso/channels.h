#ifndef CHANNELS_H
#define CHANNELS_H
#include "eeprom/eeprom.h"
#include "preferences.h"
//---------------------------------------
//#define CHANNEL_NUMBER	3//���������� �������
//---------------------------------------
struct Channel
{	
	union 
	{
		struct
		{
			unsigned long DOL;
			unsigned int frequency;
			unsigned int mid_frequency;
			unsigned char state_byte;
			unsigned char CRC; 
		} channels;
	
		unsigned char i2c_buf[10];//��������������� ����� ���������
	} I2C_CHNL;
	unsigned char transfer;
	unsigned long DOL_buf;		  //�������� ����������
	unsigned int frequency_buf;
	unsigned int mid_frequency_buf;

} ;
//---------------------------------------
//void ChannelsInit(void);//������������� ��������� �������
//void Store_Channels_Data(void);//�������� ������ ������� � ����
//void Restore_Channels_Data(void);//����������� ������ ������� �� ����

//---------------------------------------
#endif