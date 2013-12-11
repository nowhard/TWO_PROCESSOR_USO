#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <ADuC845.h>
#include "pt/pt.h"
//-------watchdog`s interwals------------
#define WDT_125		0x32
#define WDT_250		0x42
#define WDT_500		0x52
#define WDT_1000 	0x62
#define WDT_2000 	0x72
//---------------------------------------
#define PROCESS_NUM	6
enum
{
	Proto_Proc=0,
	Sort_Proc= 1,
	Watch_Proc=2,
	I2C_RepeatRead_Proc=3,
	Frequency_Measure_Proc=4,
	I2C_Proc=5
};

enum
{
	RUN=0,
	IDLE=1	
};

struct watchdog
{
  unsigned char process_state;
  unsigned int count;
};

extern struct watchdog wdt_count[];

void WDT_Init(unsigned char interval);//установка интервала срабатывания сторожевого таймера, запуск
void WDT_Clear(void);//сброс счетчика сторожевого таймера
 PT_THREAD(WDT_Process(struct pt *pt));

#endif