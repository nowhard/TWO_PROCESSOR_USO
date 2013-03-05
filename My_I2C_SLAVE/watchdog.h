#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <ADuC845.h>
//-------watchdog`s interwals------------
#define WDT_125		0x32
#define WDT_250		0x42
#define WDT_500		0x52
#define WDT_1000 	0x62
#define WDT_2000 	0x72
//---------------------------------------

void WDT_Init(unsigned char interval);//��������� ��������� ������������ ����������� �������, ������
void WDT_Clear(void);//����� �������� ����������� �������


#endif