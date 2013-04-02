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

#define I2C_ADDR 0x58//0xD0	

extern unsigned char idata i2c_buffer[6];

extern struct I2C_Channel xdata i2c_channels;
extern struct pt pt_proto;
volatile struct pt pt_i2c_read, pt_freq_measure,pt_sort,pt_i2c_process;


//-----------------------------------------
void Set_Handlers(void);//установить обработчики событий
PT_THREAD(I2C_RepeatRead(struct pt *pt));
 //---------------------------------------

void main(void) //using 0
{			   
	EA = 0;
	
	//CFG845=0x1;//enable xram
	
	PLLCON&=PLLCON_VAL;//настройка частоты процессора
	
	ChannelsInit();//инициализаци€ настроек каналов
	Protocol_Init();	
	Timer1_Initialize(); //таймер шедулера 200√ц	
	ADC_Initialize();
	UART_Init();
	Frequency_Init();

	WDT_Init(WDT_250);//включить сторожевой таймер
	I2C_Init();


	RestoreCalibrate();

	PT_INIT(&pt_i2c_read);
	PT_INIT(&pt_freq_measure);
	PT_INIT(&pt_sort);
	PT_INIT(&pt_i2c_process);

	EA=1;

	i2c_buffer[0]=0x0;//сброс флага инициализации
//	I2C_Write_Buf(I2C_ADDR,&buf,1);

	I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,1,i2c_channels.I2C_CHNL.i2c_buf,10);	  //производим первое чтение заранее
	while(1)
	{	
		ProtoProcess(&pt_proto);
		I2C_RepeatRead(&pt_i2c_read);
		Frequency_Measure_Process(&pt_freq_measure);	
		ulongsort_process(&pt_sort);
		I2C_Process(&pt_i2c_process);
		WDT_Clear();	    
	}
}
//-----------------------------------------------------------------------------
 PT_THREAD(I2C_RepeatRead(struct pt *pt))//поток чтени€ I2C
 {  
	   PT_BEGIN(pt);
	
	  while(1) 
	  {
			PT_DELAY(pt,15);
			//buf[0]=0x0;  	//номер канала, который читаем с I2C устройства
			I2C_Repeat_Start_Read(I2C_ADDR,&i2c_buffer,1,i2c_channels.I2C_CHNL.i2c_buf,10);	//исправить сдвиг адресации			
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
	return;	
}