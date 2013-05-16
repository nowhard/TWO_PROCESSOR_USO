#include "frequency.h"

#define FREQ_CHANNELS	3


#define FRAME_TIME_1	511  //границы кадров захвата
#define FRAME_TIME_2	1023
#define FRAME_TIME_3	1535
#define FRAME_TIME_4	2047

#define SHEAR			9	  //рассчитывается из максимума кадра захвата, как Nбит-2

# define CLI_EXT  EX1=0; EX0=0; ET0=0;
# define STI_EXT  EX1=1; EX0=1; ET0=1;


//------------------------------------------
volatile struct Frequency xdata frequency[FREQ_CHANNELS]={0}; //структура частотных каналов
//------------------------------------------
#define FRQ_CHNL_1 0
#define FRQ_CHNL_2 1
#define FRQ_CHNL_3 2
//--------------------------------------------------------
#pragma OT(9,Speed)
void Frequency_Init(void) //инициализация частотных каналов
{
	unsigned char i=0,j=0;
	
	Timer0_Initialize();
	Timer2_Initialize();
	EX1=1;//включаем внешние прерывания
	EX0=1;
	IT0=1;
	IT1=1;
	

	for(i=0;i<3;i++) //обнуление структуры
	{
		frequency[i].time_counter=0;
		for(j=0;j<4;j++)
		{
			frequency[i].frame[j].event_counter=0;
			frequency[i].frame[j].timestamp=0;
			frequency[i].frame[j].time_copy=0;
			frequency[i].frame[j].event_copy=0;
		}
	}
	return;
}
//--------------------------------------------------------
void INT0_ISR(void) interrupt 0 //using 3//обработчик внешнего прерывания 0
{
	unsigned char cnt;
	cnt=(frequency[FRQ_CHNL_1].time_counter>>SHEAR)&0x3;
	frequency[FRQ_CHNL_1].frame[cnt].event_counter++;
	frequency[FRQ_CHNL_1].frame[cnt].timestamp=frequency[FRQ_CHNL_1].time_counter;
	return;
}
//--------------------------------------------------------
void INT1_ISR(void) interrupt 2 //using 3//обработчик внешнего прерывания 1
{
 	unsigned char cnt;
	cnt=(frequency[FRQ_CHNL_1].time_counter>>SHEAR)&0x3;
	frequency[FRQ_CHNL_2].frame[cnt].event_counter++;
	frequency[FRQ_CHNL_2].frame[cnt].timestamp=frequency[FRQ_CHNL_1].time_counter;
	return;
}
//--------------------------------------------------------
void INT2_ISR(void) interrupt 1 //using 3//обработчик внешнего прерывания 2-использует внешний вход таймера T0
{
	unsigned char cnt;
		
	TH0=0xFF;
	TL0=0xFF;

	cnt=(frequency[FRQ_CHNL_1].time_counter>>SHEAR)&0x3;

	frequency[FRQ_CHNL_3].frame[cnt].event_counter++;
	frequency[FRQ_CHNL_3].frame[cnt].timestamp=frequency[FRQ_CHNL_1].time_counter;
	return;
}
//--------------------------------------------------------
void Timer2_ISR(void) interrupt 5 //using 3//обработчик прерывания счетного таймера частоты
{
	TF2 = 0;
//	TH2=0xE7;
//	TL2=0xFF;

 	switch(frequency[FRQ_CHNL_1].time_counter)
	{
		case FRAME_TIME_1:
		{
			frequency[FRQ_CHNL_1].frame[0].time_copy=frequency[FRQ_CHNL_1].frame[0].timestamp+FRAME_TIME_4-frequency[FRQ_CHNL_1].frame[3].timestamp;
			frequency[FRQ_CHNL_1].frame[0].event_copy=frequency[FRQ_CHNL_1].frame[0].event_counter;	


			frequency[FRQ_CHNL_1].frame[3].event_counter=0;
			//---------------------------------------
			frequency[FRQ_CHNL_2].frame[0].time_copy=frequency[FRQ_CHNL_2].frame[0].timestamp+FRAME_TIME_4-frequency[FRQ_CHNL_2].frame[3].timestamp;
			frequency[FRQ_CHNL_2].frame[0].event_copy=frequency[FRQ_CHNL_2].frame[0].event_counter;	


			frequency[FRQ_CHNL_2].frame[3].event_counter=0;

			//---------------------------------------
			frequency[FRQ_CHNL_3].frame[0].time_copy=frequency[FRQ_CHNL_3].frame[0].timestamp+FRAME_TIME_4-frequency[FRQ_CHNL_3].frame[3].timestamp;
			frequency[FRQ_CHNL_3].frame[0].event_copy=frequency[FRQ_CHNL_3].frame[0].event_counter;	


			frequency[FRQ_CHNL_3].frame[3].event_counter=0;

		}
		break;

		case FRAME_TIME_2:
		{
			frequency[FRQ_CHNL_1].frame[1].time_copy=frequency[FRQ_CHNL_1].frame[1].timestamp-frequency[FRQ_CHNL_1].frame[0].timestamp;
			frequency[FRQ_CHNL_1].frame[1].event_copy=frequency[FRQ_CHNL_1].frame[1].event_counter;	

			frequency[FRQ_CHNL_1].frame[0].event_counter=0;
			//----------------------------------------------
			frequency[FRQ_CHNL_2].frame[1].time_copy=frequency[FRQ_CHNL_2].frame[1].timestamp-frequency[FRQ_CHNL_2].frame[0].timestamp;
			frequency[FRQ_CHNL_2].frame[1].event_copy=frequency[FRQ_CHNL_2].frame[1].event_counter;	

			frequency[FRQ_CHNL_2].frame[0].event_counter=0;
			//----------------------------------------------
			frequency[FRQ_CHNL_3].frame[1].time_copy=frequency[FRQ_CHNL_3].frame[1].timestamp-frequency[FRQ_CHNL_3].frame[0].timestamp;
			frequency[FRQ_CHNL_3].frame[1].event_copy=frequency[FRQ_CHNL_3].frame[1].event_counter;	

			frequency[FRQ_CHNL_3].frame[0].event_counter=0;
		}
		break;

		case FRAME_TIME_3:
		{
			frequency[FRQ_CHNL_1].frame[2].time_copy=frequency[FRQ_CHNL_1].frame[2].timestamp-frequency[FRQ_CHNL_1].frame[1].timestamp;
			frequency[FRQ_CHNL_1].frame[2].event_copy=frequency[FRQ_CHNL_1].frame[2].event_counter;	

			frequency[FRQ_CHNL_1].frame[1].event_counter=0;
			//----------------------------------------------------
			frequency[FRQ_CHNL_2].frame[2].time_copy=frequency[FRQ_CHNL_2].frame[2].timestamp-frequency[FRQ_CHNL_2].frame[1].timestamp;
			frequency[FRQ_CHNL_2].frame[2].event_copy=frequency[FRQ_CHNL_2].frame[2].event_counter;	

			frequency[FRQ_CHNL_2].frame[1].event_counter=0;
			//---------------------------------------------------
			frequency[FRQ_CHNL_3].frame[2].time_copy=frequency[FRQ_CHNL_3].frame[2].timestamp-frequency[FRQ_CHNL_3].frame[1].timestamp;
			frequency[FRQ_CHNL_3].frame[2].event_copy=frequency[FRQ_CHNL_3].frame[2].event_counter;	

			frequency[FRQ_CHNL_3].frame[1].event_counter=0;
		}
		break;

		case FRAME_TIME_4:
		{
			frequency[FRQ_CHNL_1].frame[3].time_copy=frequency[FRQ_CHNL_1].frame[3].timestamp-frequency[FRQ_CHNL_1].frame[2].timestamp;
			frequency[FRQ_CHNL_1].frame[3].event_copy=frequency[FRQ_CHNL_1].frame[3].event_counter;	

			frequency[FRQ_CHNL_1].frame[2].event_counter=0;	
			//-----------------------------------------------------
			frequency[FRQ_CHNL_2].frame[3].time_copy=frequency[FRQ_CHNL_2].frame[3].timestamp-frequency[FRQ_CHNL_2].frame[2].timestamp;
			frequency[FRQ_CHNL_2].frame[3].event_copy=frequency[FRQ_CHNL_2].frame[3].event_counter;	

			frequency[FRQ_CHNL_2].frame[2].event_counter=0;
			//----------------------------------------------------
			frequency[FRQ_CHNL_3].frame[3].time_copy=frequency[FRQ_CHNL_3].frame[3].timestamp-frequency[FRQ_CHNL_3].frame[2].timestamp;
			frequency[FRQ_CHNL_3].frame[3].event_copy=frequency[FRQ_CHNL_3].frame[3].event_counter;	

			frequency[FRQ_CHNL_3].frame[2].event_counter=0;
			
			frequency[FRQ_CHNL_1].time_counter=0;
		}
		break;
	}

	frequency[FRQ_CHNL_1].time_counter++;
	frequency[FRQ_CHNL_1].time_counter&=FRAME_TIME_4;

	return;
}
//------------------------------------------------------------
#pragma OT(0,Speed)
  PT_THREAD(Frequency_Measure_Process(struct pt *pt))
 { 
 	static unsigned char frq_chnl=0;
	static unsigned long temp_freq=0; 
   PT_BEGIN(pt);

  while(1) 
  {
		PT_DELAY(pt,40);
		temp_freq=(((unsigned long)frequency[frq_chnl].frame[0].event_copy+(unsigned long)frequency[frq_chnl].frame[1].event_copy+(unsigned long)frequency[frq_chnl].frame[2].event_copy+(unsigned long)frequency[frq_chnl].frame[3].event_copy)<<18)/((frequency[frq_chnl].frame[0].time_copy+frequency[frq_chnl].frame[1].time_copy+frequency[frq_chnl].frame[2].time_copy+frequency[frq_chnl].frame[3].time_copy));

		PT_YIELD(pt);//дадим другим процессам время
		if(temp_freq<=0xFFFF)
		{
			channels[8+frq_chnl].channel_data=temp_freq;
		}
		else
		{
			channels[8+frq_chnl].channel_data=0xFFFF;
		}
	
	
		frq_chnl++;
		if(frq_chnl>=3)
		{
			frq_chnl=0;
		} 

  }
  PT_END(pt);

 }