#include "channels.h"


//-----------------------------------
volatile struct Channel xdata channels[CHANNEL_NUMBER] /*_at_ 0x20*/;
//-----------------------------------
void ChannelsInit(void) //using 0//������������� ��������� �������
{
	Restore_Channels_Data();
 //-----------------------------------------------------------------------
	if(channels[0].settings.set.type!=0 || channels[1].settings.set.type!=0)
	{	
		channels[0].settings.set.modific=CHNL_ADC_FIX_16;
		channels[0].settings.set.state_byte_1=0x40;
		channels[0].settings.set.state_byte_2=0x1E;
		channels[0].channel_data_calibrate=0;
		channels[0].calibrate.cal.calibrate=0;
		// ����� 2 - ������������ ���
		channels[1].settings.set.modific=CHNL_ADC_FIX_16;
		channels[1].settings.set.state_byte_1=0x40;
		channels[1].settings.set.state_byte_2=0x1E;
		channels[1].channel_data_calibrate=0;
		channels[1].calibrate.cal.calibrate=0;
		// ����� 3 - ������������ ���
		channels[2].settings.set.modific=CHNL_ADC_FIX_16;
		channels[2].settings.set.state_byte_1=0x40;
		channels[2].settings.set.state_byte_2=0x1E;
		channels[2].channel_data_calibrate=0;
		channels[2].calibrate.cal.calibrate=0;
		// ����� 4 - ������������ ���
		channels[3].settings.set.modific=CHNL_ADC_FIX_16;
		channels[3].settings.set.state_byte_1=0x40;
		channels[3].settings.set.state_byte_2=0x1E;
		channels[3].channel_data_calibrate=0;
		channels[3].calibrate.cal.calibrate=0;
		// ����� 5 - ������������ ���
		channels[4].settings.set.modific=CHNL_ADC_FIX_16;
		channels[4].settings.set.state_byte_1=0x40;
		channels[4].settings.set.state_byte_2=0x1E;
		channels[4].channel_data_calibrate=0;
		channels[4].calibrate.cal.calibrate=0;
		// ����� 6 - ������������ ���
		channels[5].settings.set.modific=CHNL_ADC_FIX_16;
		channels[5].settings.set.state_byte_1=0x40;
		channels[5].settings.set.state_byte_2=0x1E;
		channels[5].channel_data_calibrate=0;
		channels[5].calibrate.cal.calibrate=0;
		// ����� 7 - ������������ ���
		channels[6].settings.set.modific=CHNL_ADC_FIX_16;
		channels[6].settings.set.state_byte_1=0x40;
		channels[6].settings.set.state_byte_2=0x1E;
		channels[6].channel_data_calibrate=0;
		channels[6].calibrate.cal.calibrate=0;
		// ����� 8 - ������������ ���
		channels[7].settings.set.modific=CHNL_ADC_FIX_16;
		channels[7].settings.set.state_byte_1=0x40;
		channels[7].settings.set.state_byte_2=0x1E;
		channels[7].channel_data_calibrate=0;
		channels[7].calibrate.cal.calibrate=0;
		// ����� 9 - ���������� 0-256 ��
		channels[8].settings.set.state_byte_1=0x40;
		channels[8].settings.set.state_byte_2=0x0A;
		channels[8].calibrate.cal.calibrate=0;
		// ����� 10 - ���������� 0-256 ��
		channels[9].settings.set.state_byte_1=0x40;
		channels[9].settings.set.state_byte_2=0x0A;
		channels[9].calibrate.cal.calibrate=0;
		// ����� 11 - ���������� 0-256 ��
		channels[10].settings.set.state_byte_1=0x40;
		channels[10].settings.set.state_byte_2=0x0A;
		channels[10].calibrate.cal.calibrate=0;
		// ����� 12 - ��� I2C
		channels[11].settings.set.state_byte_1=0x40;
		channels[11].settings.set.state_byte_2=0x0A;
		channels[11].calibrate.cal.calibrate=0;
				// ����� 13 �������
		channels[12].settings.set.state_byte_1=0x40;
		channels[12].settings.set.state_byte_2=0x0A;
				// ����� 14 ������� �������
		channels[13].settings.set.state_byte_1=0x40;
		channels[13].settings.set.state_byte_2=0x0A;
	}
 		
		
		channels[0].number=0;		  
		channels[0].settings.set.type=CHNL_ADC;	
		channels[0].channel_data=0;
		// ����� 2 - ������������ ���
		channels[1].number=0;
		channels[1].settings.set.type=CHNL_ADC;
		channels[1].channel_data=0;
		// ����� 3 - ������������ ���
		channels[2].number=0;		 
		channels[2].settings.set.type=CHNL_ADC;
		channels[2].channel_data=0;
		// ����� 4 - ������������ ���
		channels[3].number=0;		 
		channels[3].settings.set.type=CHNL_ADC;
		channels[3].channel_data=0;
		// ����� 5 - ������������ ���
		channels[4].number=0xFF;		 
		channels[4].settings.set.type=CHNL_ADC;
		channels[4].channel_data=0;
		// ����� 6 - ������������ ���
		channels[5].number=0xFF;		 
		channels[5].settings.set.type=CHNL_ADC;
		channels[5].channel_data=0;
		// ����� 7 - ������������ ���
		channels[6].number=0xFF;		 
		channels[6].settings.set.type=CHNL_ADC;
		channels[6].channel_data=0;
		// ����� 8 - ������������ ���
		channels[7].number=0xFF;		 
		channels[7].settings.set.type=CHNL_ADC;
		channels[7].channel_data=0;
		// ����� 9 - ���������� 0-256 ��
		channels[8].number=0xFF;		 
		channels[8].settings.set.type=CHNL_FREQ;
		channels[8].settings.set.modific=CHNL_FREQ_256;
		channels[8].channel_data=0x0;
		// ����� 10 - ���������� 0-256 ��
		channels[9].number=0xFF;		 
		channels[9].settings.set.type=CHNL_FREQ;
		channels[9].settings.set.modific=CHNL_FREQ_256;
		channels[9].channel_data=0x0;
		// ����� 11 - ���������� 0-256 ��
		channels[10].number=0xFF;		 
		channels[10].settings.set.type=CHNL_FREQ;
		channels[10].settings.set.modific=CHNL_FREQ_256;
		channels[10].channel_data=0x0;
		// ����� 12 - ��� I2C
		channels[11].number=0xFF;		 
		channels[11].settings.set.type=CHNL_DOL;
		channels[11].settings.set.modific=CHNL_DOL_ENC;
		channels[11].channel_data=0x80008000;;
				// ����� 13 �������
		channels[12].number=0;		 
		channels[12].settings.set.type=CHNL_FREQ;
		channels[12].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[12].channel_data=0x0;
				// ����� 14 ������� �������
		channels[13].number=0;		 
		channels[13].settings.set.type=CHNL_FREQ;
		channels[13].settings.set.modific=CHNL_FREQ_COUNT_T;
		channels[13].channel_data=0x0;

	return;
}
//-----------------------------------
void Store_Channels_Data(void) //using 0//�������� ������ ������� � ����
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Write(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}
	return;
}
//-----------------------------------
void Restore_Channels_Data(void) //using 0//����������� ������ ������� �� ����
{
	unsigned char i=0;
	for(i=0;i<CHANNEL_NUMBER;i++)
	{
		EEPROM_Read(&channels[i].settings.serialize,1,ADC_SETTINGS_ADDR+i);
	}
	return;
}
