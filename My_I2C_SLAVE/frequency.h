#ifndef FREQUENCY_H
#define FREQUENCY_H
#include "rtos/core.h"
#include "timer0.h"
#include "timer2.h"
#include "proto_uso/channels.h"
#include "preferences.h"
#include "watchdog.h"
//----------------------------------------------------
void Frequency_Init(void);//������������� ��������� �������
unsigned char Frequency_Measure_Process(void);//����������� ������� ��������� �������
unsigned char Sec_Task(void);//��������� ������ ��� ����������� ������� �������
void INT0_ISR(void);//���������� �������� ���������� 0
void INT1_ISR(void);//���������� �������� ���������� 1
void INT2_ISR(void);//���������� �������� ���������� 2-���������� ������� ���� ������� T0
void Timer2_ISR(void);//���������� ���������� �������� ������� �������
//----------------------------------------------------
extern struct Channel xdata channels;
//----------------------------------------------------
#endif