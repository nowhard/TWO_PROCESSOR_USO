#ifndef CHANNELS_H
#define CHANNELS_H
#include "eeprom/eeprom.h"
#include "preferences.h"
//---------------------------------------
//#define CHANNEL_NUMBER	3//���������� �������
#define CHNL_ADC	0//����� ���
	#define CHNL_ADC_FIX_16	0
	#define CHNL_ADC_SW_16	1
	#define CHNL_ADC_FIX_24	3
	#define CHNL_ADC_SW_24	2
#define CHNL_DOL	1 //����� ���
	#define CHNL_DOL_ENC	0
	#define CHNL_DOL_FLOAT	1
#define CHNL_FREQ	2//���������/�������
	#define	 CHNL_FREQ_COUNT_T	0
	#define	 CHNL_FREQ_256		1
	#define  CHNL_FREQ_PERIOD	3
	#define	 CHNL_FREQ_COUNT	4
//---------------------------------------
#define I2C_FRAME_SIZE	12
struct Channel
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
	
		unsigned char i2c_buf[I2C_FRAME_SIZE];//��������������� ����� ���������
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