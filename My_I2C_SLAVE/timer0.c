#include "timer0.h"


void Timer0_Initialize(void) //using 0
{
	TMOD &= 0xF0; // 1111 0000 �������
	TMOD |= 0x5; // ������ � ����� �������� 16 �������� 

	TH0	= TH0_VAL; /// 
	TL0 = TL0_VAL;//
	

	ET0=1;
	TR0=1;
	return;
}


