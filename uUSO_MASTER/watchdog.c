#include "watchdog.h"

//---------------------global variables-----------------------------------
volatile unsigned char data wdt_intval;
struct watchdog wdt_count[PROCESS_NUM];
volatile struct pt pt_wdt;
//------------------------------------------------------------------------
void WDT_Init(unsigned char interval) //using 0//установка интервала срабатывания сторожевого таймера, запуск	//небезопасная
{
	unsigned char i=0;	

	wdt_intval=interval;	
	WDWR=1;//WDWR=1-разрешение записи в регистр
	WDCON=wdt_intval;

//	wdt_count[Proto_Proc].process_state=IDLE;
//	wdt_count[Proto_Proc].count=0;
//	wdt_count[Sort_Proc].process_state=IDLE;
//	wdt_count[Sort_Proc].count=0;
//	wdt_count[Watch_Proc].process_state=IDLE;
//	wdt_count[Watch_Proc].count=0;
//
//	wdt_count[I2C_RepeatRead_Proc].process_state=IDLE;
//	wdt_count[I2C_RepeatRead_Proc].count=0;
//	wdt_count[Frequency_Measure_Proc].process_state=IDLE;
//	wdt_count[Frequency_Measure_Proc].count=0;
//	wdt_count[I2C_Proc].process_state=IDLE;
//	wdt_count[I2C_Proc].count=0;

	for(i=0;i<PROCESS_NUM;i++)
	{
		 wdt_count[i].count=0;
		 wdt_count[i].process_state=IDLE;
	}

	PT_INIT(&pt_wdt);
	return;
}
//------------------------------------------------------------------------
void WDT_Clear(void) //using 0//сброс счетчика сторожевого таймера
{
	EA=0;

	WDWR=1;//WDWR=1-разрешение записи в регистр
	WDCON=wdt_intval;	

	EA=1;
	return;
}
//------------------------------------------------------------------------
 PT_THREAD(WDT_Process(struct pt *pt))
 {
   
  volatile unsigned char i=0, process_error=0;
  wdt_count[Watch_Proc].process_state=RUN;

  PT_BEGIN(pt);

  while(1) 
  {
  		PT_DELAY(pt,100);
		process_error=0;
		
		wdt_count[Watch_Proc].count++;

		for(i=0;i<PROCESS_NUM;i++)
		{
			if((wdt_count[i].count|wdt_count[i].process_state)==0)
			{
				process_error=1;
				break;
			}
			else
			{
				wdt_count[i].count=0;
			}
		}
		
		if(process_error==0)//все процессы работают
		{
				WDT_Clear();	
		}	
  }
  PT_END(pt);
}