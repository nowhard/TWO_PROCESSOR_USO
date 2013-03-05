#include "frequency.h"

#define FREQ_CHANNELS	3

# define CLI_EXT  EX1=0; EX0=0; ET0=0;
# define STI_EXT  EX1=1; EX0=1; ET0=1;


//-------------------------------------------------------- debug
unsigned char volatile idata  timer_busy=0;	   //���� ��������� �������
unsigned long volatile idata counter_32; //���� ������ ������������, �������������� ���� �������
unsigned long volatile idata freq_buffer[FREQ_CHANNELS]={0};
unsigned int volatile  idata channel_counter[3]={0};//��������� ������� ������

unsigned char volatile idata measure_state=0;//������� ��������� ��� ����������� �������� ����������(���������� �������� �� ���������)

#define TIMER_FREE 0  //������ �������� ��� �������
#define BUSY_CHN1  1
#define BUSY_CHN2  2
#define BUSY_CHN3  3

#define FRQ_CHNL_1 0
#define FRQ_CHNL_2 1
#define FRQ_CHNL_3 2
//--------------------------------------------------------
void Frequency_Init(void) //������������� ��������� �������
{
	Timer0_Initialize();
	Timer2_Initialize();
	EX1=1;//�������� ������� ����������
	EX0=1;
	IT0=1;
	IT1=1;
	return;
}
//--------------------------------------------------------
unsigned char Frequency_Measure_Process(void) //using 2//����������� ������� ��������� �������
{
	if(counter_32>0xC00000  && timer_busy)//���� ������� ����� �� ��������, ����������
	{
		TR2=0;
		timer_busy=TIMER_FREE;
		TH2=TL2=0;
		freq_buffer[timer_busy-1]=0xFFFFFFFF;
	} 

	switch(measure_state)
	{
		case 0:
		{
			channels[8].channel_data=((F_OSC_12)/ (freq_buffer[FRQ_CHNL_1]));
			measure_state=1;
		}
		break;

		case 1:
		{
			channels[9].channel_data=(F_OSC_12)/ (freq_buffer[FRQ_CHNL_2]);
			measure_state=2;
		}
		break;

		case 2:
		{
			channels[10].channel_data=((F_OSC_12)/ (freq_buffer[FRQ_CHNL_3]));
			measure_state=0;
		}
		break;
	}
		
	setTimer(FREQ_TASK, 0, 50);
	return 0;
}
//--------------------------------------------------------
void INT0_ISR(void) interrupt 0 //using 3//���������� �������� ���������� 0
{
	CLI_EXT
	channel_counter[FRQ_CHNL_1]++;
	if(timer_busy==BUSY_CHN1) //���� ��� ����� ���� ��������
	{	

		TR2=0;
		// ���
		freq_buffer[FRQ_CHNL_1]=(unsigned int)TH2<<8;
		freq_buffer[FRQ_CHNL_1]+=(TL2+counter_32);
		timer_busy=TIMER_FREE;
		STI_EXT
		return;	
	}
	if( timer_busy==TIMER_FREE)
	{
		//���
		TH2=TL2=0;
		TR2=1;//��������� ������
		counter_32=0;
		timer_busy=BUSY_CHN1;
		STI_EXT
		return;
	} 
	STI_EXT
	return;
}
//--------------------------------------------------------
void INT1_ISR(void) interrupt 2 //using 3//���������� �������� ���������� 1
{
	CLI_EXT
	channel_counter[FRQ_CHNL_2]++;
	if(timer_busy==BUSY_CHN2) //���� ��� ����� ���� ��������
	{
		TR2=0;
		// ���
		freq_buffer[FRQ_CHNL_2]=(unsigned int)TH2<<8;
		freq_buffer[FRQ_CHNL_2]+=(TL2+counter_32);
		timer_busy=TIMER_FREE;
		STI_EXT
		return;	
	}
	if( timer_busy==TIMER_FREE)
	{
		//���
		TH2=TL2=0;
		TR2=1;//��������� ������
		counter_32=0;
		timer_busy=BUSY_CHN2;
		STI_EXT
		return;
	}
	STI_EXT
	
	return;
}
//--------------------------------------------------------
void INT2_ISR(void) interrupt 1 //using 3//���������� �������� ���������� 2-���������� ������� ���� ������� T0
{
	CLI_EXT
	TH0=TL0=0xFF;
	channel_counter[FRQ_CHNL_3]++;
	if(timer_busy==BUSY_CHN3) //���� ��� ����� ���� ��������
	{
		TR2=0;
		// ���
		freq_buffer[FRQ_CHNL_3]=(unsigned int)TH2<<8;
		freq_buffer[FRQ_CHNL_3]+=(TL2+counter_32);
		timer_busy=TIMER_FREE;

		STI_EXT
		
		return;	
	}
	if( timer_busy==TIMER_FREE)
	{
		//���
		TH2=TL2=0;
		TR2=1;//��������� ������
		counter_32=0;
		timer_busy=BUSY_CHN3;
		STI_EXT
		
		return;
	} 
	
	STI_EXT
	
	return;
}
//--------------------------------------------------------
void Timer2_ISR(void) interrupt 5 //using 3//���������� ���������� �������� ������� �������
{
	TF2 = 0;
	TH2=TL2=0;
	counter_32+=0x10000;	//��������� 17 ��������
	return;
}
//------------------------------------------------------------
unsigned char Sec_Task(void) //using 2//��������� ������ ��� ����������� ������� �������
{	
	 	unsigned char idata i=0;
	for(i=0;i<FREQ_CHANNELS;i++)
	{
		if(!channel_counter[i])
		{
			freq_buffer[i]=0xFFFFFFFF;
		}
		channel_counter[i]=0;
	} 
	WDT_Clear();
	//--------------------
	setTimer(SEC_TASK, 0, 200);
	return 0;
}