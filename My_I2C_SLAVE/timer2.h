#ifndef TIMER2_H
#define TIMER2_H

#include <ADuC845.h>
#include "preferences.h"
#include "channels.h"
//-----------------------------------
#define FREQ_CHN		1
#define FREQ_CHN_MID	2

void Timer2_Initialize(void);// инициализация таймера 2-счетного таймера для частотомера
void Frequency_Handler(void);//обработчик измерения частоты
//------------------------------------
extern struct Channel xdata channels;
#endif