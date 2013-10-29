#include "channels.h"
#include "crc_table.h"

//-----------------------------------
volatile struct Channel xdata channels[CHANNEL_NUMBER] /*_at_ 0x20*/;
extern unsigned char code Crc8Table[];

unsigned char channel_number=CHANNEL_NUMBER;//���������� �������
//-----------------------------------
void ChannelsInit(void) //using 0//������������� ��������� �������
{	
	Restore_Channels_Data();
	channels[0].channel_data=0;
	channels[1].channel_data=0;
	channels[2].channel_data=0;
	channels[3].channel_data=0;
	channels[4].channel_data=0;
	channels[5].channel_data=0;
	channels[6].channel_data=0;
	channels[7].channel_data=0;
	channels[8].channel_data=0;
	channels[9].channel_data=0;
	channels[10].channel_data=0;
	channels[11].channel_data=0x80008000;
	channels[12].channel_data=0;
	channels[13].channel_data=0;
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

		channels[0].number=0;		 
		channels[0].settings.set.type=CHNL_ADC;
		channels[0].settings.set.modific=CHNL_ADC_FIX_24;
		channels[0].settings.set.state_byte_1=0x40;
		channels[0].settings.set.state_byte_2=0x06;
		channels[0].channel_data=0;
		channels[0].channel_data_calibrate=0;
	//	channels[0].calibrate.cal.calibrate=0;
		// ����� 2 - ������������ ���
		channels[1].number=1;
		channels[1].settings.set.type=CHNL_ADC;
		channels[1].settings.set.modific=CHNL_ADC_FIX_24;
		channels[1].settings.set.state_byte_1=0x40;
		channels[1].settings.set.state_byte_2=0x06;
		channels[1].channel_data=0;
		channels[1].channel_data_calibrate=0;
	//	channels[1].calibrate.cal.calibrate=0;
		// ����� 3 - ������������ ���
		channels[2].number=2;		 
		channels[2].settings.set.type=CHNL_ADC;
		channels[2].settings.set.modific=CHNL_ADC_FIX_24;
		channels[2].settings.set.state_byte_1=0x40;
		channels[2].settings.set.state_byte_2=0x06;
		channels[2].channel_data=0;
		channels[2].channel_data_calibrate=0;
	//	channels[2].calibrate.cal.calibrate=0;
		// ����� 4 - ������������ ���
		channels[3].number=3;		 
		channels[3].settings.set.type=CHNL_ADC;
		channels[3].settings.set.modific=CHNL_ADC_FIX_24;
		channels[3].settings.set.state_byte_1=0x40;
		channels[3].settings.set.state_byte_2=0x06;
		channels[3].channel_data=0;
		channels[3].channel_data_calibrate=0;
	//	channels[3].calibrate.cal.calibrate=0;
		// ����� 5 - ������������ ���
		channels[4].number=4;		 
		channels[4].settings.set.type=CHNL_ADC;
		channels[4].settings.set.modific=CHNL_ADC_FIX_24;
		channels[4].settings.set.state_byte_1=0x40;
		channels[4].settings.set.state_byte_2=0x06;
		channels[4].channel_data=0;
		channels[4].channel_data_calibrate=0;
	//	channels[4].calibrate.cal.calibrate=0;
		// ����� 6 - ������������ ���
		channels[5].number=5;		 
		channels[5].settings.set.type=CHNL_ADC;
		channels[5].settings.set.modific=CHNL_ADC_FIX_24;
		channels[5].settings.set.state_byte_1=0x40;
		channels[5].settings.set.state_byte_2=0x06;
		channels[5].channel_data=0;
		channels[5].channel_data_calibrate=0;
	//	channels[5].calibrate.cal.calibrate=0;
		// ����� 7 - ������������ ���
		channels[6].number=6;		 
		channels[6].settings.set.type=CHNL_ADC;
		channels[6].settings.set.modific=CHNL_ADC_FIX_24;
		channels[6].settings.set.state_byte_1=0x40;
		channels[6].settings.set.state_byte_2=0x06;
		channels[6].channel_data=0;
		channels[6].channel_data_calibrate=0;
	//	channels[6].calibrate.cal.calibrate=0;
		// ����� 8 - ������������ ���
		channels[7].number=7;		 
		channels[7].settings.set.type=CHNL_ADC;
		channels[7].settings.set.modific=CHNL_ADC_FIX_24;
		channels[7].settings.set.state_byte_1=0x40;
		channels[7].settings.set.state_byte_2=0x06;
		channels[7].channel_data=0;
		channels[7].channel_data_calibrate=0;
	//	channels[7].calibrate.cal.calibrate=0;
		// ����� 9 - ���������� 0-256 ��
		channels[8].number=8;		 
		channels[8].settings.set.type=CHNL_FREQ;
		channels[8].settings.set.modific=CHNL_FREQ_256;
		channels[8].settings.set.state_byte_1=0x40;
		channels[8].settings.set.state_byte_2=0x0A;
		channels[8].channel_data=0x0;
	//	channels[8].calibrate.cal.calibrate=0;
		// ����� 10 - ���������� 0-256 ��
		channels[9].number=9;		 
		channels[9].settings.set.type=CHNL_FREQ;
		channels[9].settings.set.modific=CHNL_FREQ_256;
		channels[9].settings.set.state_byte_1=0x40;
		channels[9].settings.set.state_byte_2=0x0A;
		channels[9].channel_data=0x0;
	//	channels[9].calibrate.cal.calibrate=0;
		// ����� 11 - ���������� 0-256 ��
		channels[10].number=10;		 
		channels[10].settings.set.type=CHNL_FREQ;
		channels[10].settings.set.modific=CHNL_FREQ_256;
		channels[10].settings.set.state_byte_1=0x40;
		channels[10].settings.set.state_byte_2=0x0A;
		channels[10].channel_data=0x1;
	//	channels[10].calibrate.cal.calibrate=0;
		// ����� 12 - ��� I2C
		channels[11].number=11;		 
		channels[11].settings.set.type=CHNL_DOL;
		channels[11].settings.set.modific=CHNL_DOL_ENC;
		channels[11].settings.set.state_byte_1=0x40;
		channels[11].settings.set.state_byte_2=0x0A;
		channels[11].channel_data=0x80008000;
	//	channels[11].calibrate.cal.calibrate=0;
				// ����� 13 �������
		channels[12].number=12;		 
		channels[12].settings.set.type=CHNL_FREQ;
		channels[12].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[12].settings.set.state_byte_1=0x40;
		channels[12].settings.set.state_byte_2=0x0A;
		channels[12].channel_data=0x0;

				// ����� 14 ������� �������
		channels[13].number=13;		 
		channels[13].settings.set.type=CHNL_FREQ;
		channels[13].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[13].settings.set.state_byte_1=0x40;
		channels[13].settings.set.state_byte_2=0x0A;
		channels[13].channel_data=0x0;	

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
