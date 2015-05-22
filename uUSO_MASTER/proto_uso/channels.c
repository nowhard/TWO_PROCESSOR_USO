#include "channels.h"
#include "crc_table.h"

//-----------------------------------
volatile struct Channel xdata channels[CHANNEL_NUMBER] /*_at_ 0x20*/;
extern unsigned char code Crc8Table[];

unsigned char channel_number=CHANNEL_NUMBER;//���������� �������
//-----------------------------------
void ChannelsInit(void) //������������� ��������� �������
{	
	unsigned char i=0;

	Restore_Channels_Data();

	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		channels[i].channel_data=0;
	}

//	channels[CHNL_DOL_ENC_0].settings.set.state_byte_1=0x40;
//	channels[CHNL_DOL_ENC_0].settings.set.state_byte_2=0x0A;
//	channels[CHNL_DOL_ENC_0].channel_data=0x80008000;
	return;
}
//-----------------------------------
void Store_Channels_Data(void) //using 0//�������� ������ ������� � ����
{
	unsigned char i=0;
	unsigned long crc=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Write(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}	
	crc=(unsigned long)Channels_Get_Settings_CRC();
	EEPROM_Write(&crc,1,SETTINGS_DEVICE_CRC_ADDR);//�������� CRC ��������
	return;
}
//-----------------------------------
void Restore_Channels_Data(void) //using 0//����������� ������ ������� �� ����
{
	unsigned char i=0;
	unsigned long crc=0, crc_true=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}
	crc_true=(unsigned long)Channels_Get_Settings_CRC();
 	EEPROM_Read(&crc,1,SETTINGS_DEVICE_CRC_ADDR);//������� CRC ��������

	if(crc_true!=crc)//���� eeprom, ������� ���������
	{
		Channels_Set_Default();
	}

	return;
}
//----------------------------------
void Channels_Set_Default(void)//�������� �������� ������� �� ���������
{	

		channels[CHNL_ADC_0].number=CHNL_ADC_0;		 
		channels[CHNL_ADC_0].settings.set.type=CHNL_ADC;
		channels[CHNL_ADC_0].settings.set.modific=CHNL_ADC_FIX_24;
		channels[CHNL_ADC_0].settings.set.state_byte_1=0x40;
		channels[CHNL_ADC_0].settings.set.state_byte_2=0x06;
		channels[CHNL_ADC_0].channel_data=0;
		channels[CHNL_ADC_0].channel_data_calibrate=0;
		// ����� 2 - ������������ ���
		channels[CHNL_ADC_1].number=CHNL_ADC_1;
		channels[CHNL_ADC_1].settings.set.type=CHNL_ADC;
		channels[CHNL_ADC_1].settings.set.modific=CHNL_ADC_FIX_24;
		channels[CHNL_ADC_1].settings.set.state_byte_1=0x40;
		channels[CHNL_ADC_1].settings.set.state_byte_2=0x06;
		channels[CHNL_ADC_1].channel_data=0;
		channels[CHNL_ADC_1].channel_data_calibrate=0;
		// ����� 3 - ������������ ���
		channels[CHNL_ADC_2].number=CHNL_ADC_2;		 
		channels[CHNL_ADC_2].settings.set.type=CHNL_ADC;
		channels[CHNL_ADC_2].settings.set.modific=CHNL_ADC_FIX_24;
		channels[CHNL_ADC_2].settings.set.state_byte_1=0x40;
		channels[CHNL_ADC_2].settings.set.state_byte_2=0x06;
		channels[CHNL_ADC_2].channel_data=0;
		channels[CHNL_ADC_2].channel_data_calibrate=0;
		// ����� 4 - ������������ ���
		channels[CHNL_ADC_3].number=CHNL_ADC_3;		 
		channels[CHNL_ADC_3].settings.set.type=CHNL_ADC;
		channels[CHNL_ADC_3].settings.set.modific=CHNL_ADC_FIX_24;
		channels[CHNL_ADC_3].settings.set.state_byte_1=0x40;
		channels[CHNL_ADC_3].settings.set.state_byte_2=0x06;
		channels[CHNL_ADC_3].channel_data=0;
		channels[CHNL_ADC_3].channel_data_calibrate=0;
//		// ����� 5 - ������������ ���
//		channels[CHNL_ADC_4].number=CHNL_ADC_4;		 
//		channels[CHNL_ADC_4].settings.set.type=CHNL_ADC;
//		channels[CHNL_ADC_4].settings.set.modific=CHNL_ADC_FIX_24;
//		channels[CHNL_ADC_4].settings.set.state_byte_1=0x40;
//		channels[CHNL_ADC_4].settings.set.state_byte_2=0x06;
//		channels[CHNL_ADC_4].channel_data=0;
//		channels[CHNL_ADC_4].channel_data_calibrate=0;
//		// ����� 6 - ������������ ���
//		channels[CHNL_ADC_5].number=CHNL_ADC_5;		 
//		channels[CHNL_ADC_5].settings.set.type=CHNL_ADC;
//		channels[CHNL_ADC_5].settings.set.modific=CHNL_ADC_FIX_24;
//		channels[CHNL_ADC_5].settings.set.state_byte_1=0x40;
//		channels[CHNL_ADC_5].settings.set.state_byte_2=0x06;
//		channels[CHNL_ADC_5].channel_data=0;
//		channels[CHNL_ADC_5].channel_data_calibrate=0;
//		// ����� 7 - ������������ ���
//		channels[CHNL_ADC_6].number=CHNL_ADC_6;		 
//		channels[CHNL_ADC_6].settings.set.type=CHNL_ADC;
//		channels[CHNL_ADC_6].settings.set.modific=CHNL_ADC_FIX_24;
//		channels[CHNL_ADC_6].settings.set.state_byte_1=0x40;
//		channels[CHNL_ADC_6].settings.set.state_byte_2=0x06;
//		channels[CHNL_ADC_6].channel_data=0;
//		channels[CHNL_ADC_6].channel_data_calibrate=0;
//		// ����� 8 - ������������ ���
//		channels[CHNL_ADC_7].number=CHNL_ADC_7;		 
//		channels[CHNL_ADC_7].settings.set.type=CHNL_ADC;
//		channels[CHNL_ADC_7].settings.set.modific=CHNL_ADC_FIX_24;
//		channels[CHNL_ADC_7].settings.set.state_byte_1=0x40;
//		channels[CHNL_ADC_7].settings.set.state_byte_2=0x06;
//		channels[CHNL_ADC_7].channel_data=0;
//		channels[CHNL_ADC_7].channel_data_calibrate=0;
//		// ����� 9 - ���������� 0-256 ��
//		channels[CHNL_FREQ_256_0].number=CHNL_FREQ_256_0;		 
//		channels[CHNL_FREQ_256_0].settings.set.type=CHNL_FREQ;
//		channels[CHNL_FREQ_256_0].settings.set.modific=CHNL_FREQ_256;
//		channels[CHNL_FREQ_256_0].settings.set.state_byte_1=0x40;
//		channels[CHNL_FREQ_256_0].settings.set.state_byte_2=0x0A;
//		channels[CHNL_FREQ_256_0].channel_data=0x0;
//		// ����� 10 - ���������� 0-256 ��
//		channels[CHNL_FREQ_256_1].number=CHNL_FREQ_256_1;		 
//		channels[CHNL_FREQ_256_1].settings.set.type=CHNL_FREQ;
//		channels[CHNL_FREQ_256_1].settings.set.modific=CHNL_FREQ_256;
//		channels[CHNL_FREQ_256_1].settings.set.state_byte_1=0x40;
//		channels[CHNL_FREQ_256_1].settings.set.state_byte_2=0x0A;
//		channels[CHNL_FREQ_256_1].channel_data=0x0;
//		// ����� 11 - ���������� 0-256 ��
//		channels[CHNL_FREQ_256_2].number=CHNL_FREQ_256_2;		 
//		channels[CHNL_FREQ_256_2].settings.set.type=CHNL_FREQ;
//		channels[CHNL_FREQ_256_2].settings.set.modific=CHNL_FREQ_256;
//		channels[CHNL_FREQ_256_2].settings.set.state_byte_1=0x40;
//		channels[CHNL_FREQ_256_2].settings.set.state_byte_2=0x0A;
//		channels[CHNL_FREQ_256_2].channel_data=0x1;
//		// ����� 12 - ��� I2C
//		channels[CHNL_DOL_ENC_0].number=CHNL_DOL_ENC_0;		 
//		channels[CHNL_DOL_ENC_0].settings.set.type=CHNL_DOL;
//		channels[CHNL_DOL_ENC_0].settings.set.modific=CHNL_DOL_ENC;
//		channels[CHNL_DOL_ENC_0].settings.set.state_byte_1=0x40;
//		channels[CHNL_DOL_ENC_0].settings.set.state_byte_2=0x0A;
//		channels[CHNL_DOL_ENC_0].channel_data=0x80008000;
				// ����� 13 �������
		channels[CHNL_FREQ_COUNT_T_0].number=CHNL_FREQ_COUNT_T_0;		 
		channels[CHNL_FREQ_COUNT_T_0].settings.set.type=CHNL_FREQ;
		channels[CHNL_FREQ_COUNT_T_0].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[CHNL_FREQ_COUNT_T_0].settings.set.state_byte_1=0x40;
		channels[CHNL_FREQ_COUNT_T_0].settings.set.state_byte_2=0x0A;
		channels[CHNL_FREQ_COUNT_T_0].channel_data=0x0;
				// ����� 14 ������� �������
		channels[CHNL_FREQ_COUNT_T_1].number=CHNL_FREQ_COUNT_T_1;		 
		channels[CHNL_FREQ_COUNT_T_1].settings.set.type=CHNL_FREQ;
		channels[CHNL_FREQ_COUNT_T_1].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[CHNL_FREQ_COUNT_T_1].settings.set.state_byte_1=0x40;
		channels[CHNL_FREQ_COUNT_T_1].settings.set.state_byte_2=0x0A;
		channels[CHNL_FREQ_COUNT_T_1].channel_data=0x0;	

		Store_Channels_Data();

}
//-------------------------------
unsigned char Channels_Get_Settings_CRC(void)//������ crc �������� � eerprom
{
	 unsigned char crc = 0x0;
	 unsigned char i=0,j=0;

     for(i=0;i<CHANNEL_NUMBER;i++)
	 {
        for(j=0;j<4;j++)
		{
		 	crc = Crc8Table[crc ^ (((unsigned char *)(&channels[i].settings.serialize))[j])];
		}
	 }

     return crc;
}
