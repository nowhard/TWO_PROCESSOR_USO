#ifndef FREQUENCY_H
#define FREQUENCY_H
#include "timer0.h"
#include "timer2.h"
#include "channels.h"
#include "preferences.h"
#include "watchdog.h"

//----------------------------------------------------
struct Frequency
{
	unsigned int time_counter;//������
	struct //������������� �����
	{
		unsigned int event_counter;//������� �������
		unsigned int timestamp;	   //����� �������
		unsigned int time_copy;	   //���������� ����� �� �������� �����
		unsigned int event_copy;   //���������� ������� �� �������� �����
		
	}frame[4];
};
//----------------------------------------------------
void Frequency_Init(unsigned char frequency_modific);//������������� ��������� �������
void Frequency_Measure_Process(void);//����������� ������� ��������� �������
unsigned char Sec_Task(void);//��������� ������ ��� ����������� ������� �������
void INT0_ISR(void);//���������� �������� ���������� 0
void INT1_ISR(void);//���������� �������� ���������� 1
void INT2_ISR(void);//���������� �������� ���������� 2-���������� ������� ���� ������� T0
void Timer2_ISR(void);//���������� ���������� �������� ������� �������
//----------------------------------------------------
#define FREQ_CHN		1
#define FREQ_CHN_MID	2
extern struct Channel xdata channels;
//----------------------------------------------------
#endif