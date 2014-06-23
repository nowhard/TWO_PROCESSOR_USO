#include <ADuC845.h>


#include "timer3.h"
#include "proto_uso/proto_uso.h"
#include "preferences.h"
#include "eeprom/eeprom.h"
#include "timer1.h"
#include "adc.h"
#include "watchdog.h"
#include "proto_uso/channels.h"
#include "frequency.h"

#include "i2c.h"
#include "ulongsort.h"

#include "calibrate/calibrate.h"
#include "pt/pt.h"



extern unsigned char idata i2c_buffer[6];

extern struct I2C_Channel xdata i2c_channels;
extern struct pt pt_proto, pt_wdt, pt_buf_handle;
volatile struct pt pt_i2c_read, pt_freq_measure,pt_sort,pt_i2c_process;


//-----------------------------------------
void Set_Handlers(void);//установить обработчики событий
PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
	RestoreCalibrate();
	
	ChannelsInit();//инициализаци€ настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200√ц	
	ADC_Initialize();
	UART_Init();
	Frequency_Init();

//	WDT_Init(WDT_2000);//включить сторожевой таймер
	I2C_Init();


	
	PT_INIT(&pt_i2c_read);
	PT_INIT(&pt_freq_measure);
	PT_INIT(&pt_sort);
	PT_INIT(&pt_i2c_process);

	EA=1;

	i2c_buffer[0]=0x0;//сброс флага инициализации

	I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,1,i2c_channels.I2C_CHNL.i2c_buf,sizeof(i2c_channels));	  //производим первое чтение заранее
	while(1)
	{	
		ProtoProcess(&pt_proto);
		I2C_RepeatRead(&pt_i2c_read);
		Frequency_Measure_Process(&pt_freq_measure);	
		ulongsort_process(&pt_sort);
		I2C_Process(&pt_i2c_process);
//		WDT_Process(&pt_wdt);
		RingBufHandling(&pt_buf_handle);	    
	}
}
//-----------------------------------------------------------------------------
//#pragma OT(0,Speed)
 //---------------------------------
 PT_THREAD(I2C_RepeatRead(struct pt *pt))//поток чтени€ I2C
 {  
	  wdt_count[I2C_RepeatRead_Proc].process_state=RUN;
	   
	  PT_BEGIN(pt);
	  
	  while(1) 
	  {
			PT_DELAY(pt,15);
			I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,1,i2c_channels.I2C_CHNL.i2c_buf,sizeof(i2c_channels));	//исправить сдвиг адресации
			//WDT_Clear();
			wdt_count[I2C_RepeatRead_Proc].count++;
	  }
	  PT_END(pt);

 }
//-----------------------------------
void Timer1_Interrupt(void) interrupt 3  //таймер шедулера
{
//---------------------------------------
	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//
	pt_i2c_read.pt_time++;
	pt_freq_measure.pt_time++;
	pt_sort.pt_time++;
	pt_proto.pt_time++;
	pt_wdt.pt_time++;
	pt_buf_handle.pt_time++;
	return;	
}