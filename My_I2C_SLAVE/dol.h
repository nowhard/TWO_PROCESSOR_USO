#ifndef DOL_H
#define DOL_H
#include "channels.h"
#include "preferences.h"
#include "watchdog.h"
//----------------------------------------------------
void Dol_Init(void);//������������� ��������� �������
//unsigned char Dol_Measure_Process(void);//����������� ������� ��������� �������
unsigned char Sec_Task(void);//��������� ������ ��� ����������� ������� �������
void INT0_ISR(void);//���������� �������� ���������� 0
//----------------------------------------------------
extern struct Channel xdata channels;
//----------------------------------------------------
#endif