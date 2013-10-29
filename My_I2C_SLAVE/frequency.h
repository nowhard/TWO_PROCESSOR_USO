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
	unsigned int time_counter;//таймер
	struct //измерительные кадры
	{
		unsigned int event_counter;//счетчик событий
		unsigned int timestamp;	   //штамп времени
		unsigned int time_copy;	   //полученное время за четверть кадра
		unsigned int event_copy;   //количество событий за четверть кадра
		
	}frame[4];
};
//----------------------------------------------------
void Frequency_Init(unsigned char frequency_modific);//инициализация частотных каналов
void Frequency_Measure_Process(void);//циклический процесс измерения частоты
unsigned char Sec_Task(void);//секундная задача для определения нулевой частоты
void INT0_ISR(void);//обработчик внешнего прерывания 0
void INT1_ISR(void);//обработчик внешнего прерывания 1
void INT2_ISR(void);//обработчик внешнего прерывания 2-использует внешний вход таймера T0
void Timer2_ISR(void);//обработчик прерывания счетного таймера частоты
//----------------------------------------------------
#define FREQ_CHN		1
#define FREQ_CHN_MID	2
extern struct Channel xdata channels;
//----------------------------------------------------
#endif