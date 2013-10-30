#include "frequency.h"
#include "channels.h"

#define FREQ_CHANNELS	1


#define FRAME_TIME_1	511  //границы кадров захвата
#define FRAME_TIME_2	1023
#define FRAME_TIME_3	1535
#define FRAME_TIME_4	2047

#define SHEAR			9	  //рассчитываетс€ из максимума кадра захвата, как Nбит-2

# define CLI_EXT  EX1=0; EX0=0; ET0=0;
# define STI_EXT  EX1=1; EX0=1; ET0=1;

#define TH1_VAL 0xFF //1000Hz
#define TL1_VAL 0xFF

//------------------------------------------
volatile struct Frequency xdata frequency[FREQ_CHANNELS]={0}; //структура частотных каналов
//------------------------------------------
#define FRQ_CHNL_1 0
#define FRQ_CHNL_2 1
#define FRQ_CHNL_3 2
//--------------------------------------------------------
#define Hz  10

#define kol_izmer_data 3
#define kol_sec_sred 3
#define kol_sec_mgnov 1
#define Period_sred 200
#define Period_mgnov 100

volatile unsigned long data temp_Hz_kanal_sred=0;
volatile unsigned long data temp_Hz_kanal_mgnov=0;

volatile unsigned int idata sec_kanal_sred=0;
volatile unsigned int idata sec_kanal_mgnov=0;
volatile unsigned int idata cycl_kanal=0;

volatile unsigned long idata Hz_data_sred=0;
volatile unsigned long idata Hz_data_mgnov=0;

volatile unsigned int xdata period_kanal_sred=0;
volatile unsigned int xdata period_kanal_mgnov=0;
//volatile unsigned long xdata Hz_kanal_mgnov[kol_sec_mgnov][1000/Period_mgnov]={0};
volatile unsigned long xdata Hz_kanal_sred[kol_sec_sred][1000/Period_sred]={0};

volatile unsigned long data sym_kanal_sred=0;
volatile unsigned long data sym_kanal_mgnov=0;
unsigned char poschet_intervalov=0; 

volatile unsigned char handle_freq_flag=0;
//--------------------------------------------------------
#pragma OT(9,Speed)
void Frequency_Init(unsigned char frequency_modific) //инициализаци€ частотных каналов
{
	unsigned char i=0,j=0;

	if(frequency_modific==CHNL_FREQ_256) //режим измерени€ низкой частоты
	{	
		//---------Timer1-входное прерывание--------------
		TMOD &= 0xF; // 0000 1111  очищаем
		TMOD |= 0x50; // таймер в режим счетчика 16 разр€дов 
	
		TH1	= TH1_VAL; /// 
		TL1 = TL1_VAL;//
		
	
		ET1=1;
		TR1=1;
		//---------Timer2-опорна€ частота-----------------
		ET2 = 1;
		T2CON=0x00;
	   	TL2 = 0xFF;
		TH2 = 0xE7;
		RCAP2L = 0xFF;
	    RCAP2H = 0xE7; //1024Hz
		TR2=1;
		//-----------------------
	
		for(i=0;i<FREQ_CHANNELS;i++) //обнуление структуры
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
	}
	else  //режим измерени€ высокой частоты
	{
		
		//-----------Timer1-счетный таймер----------
			TMOD=0x50;//счетчик 16 бит
			TH1	= 0; /// 
			TL1 = 0;//
			TR1=1;
		//-----------Timer2-опорный таймер----------
			ET2 = 1;
			PT2=0;//приоритет низкий
			T2CON=0x00;			  // таймер периода сбора частоты (0.01)
			RCAP2H=TH2=0x86;//0xC3;//0x86;//0x0B;	- дл€ 6,29 ћ√ц  //0x0A 
			RCAP2L=TL2=0x07;//0x03;//0x07;//0xF0; - дл€ 6,29 ћ√ц   //0x3C 
			TR2=1;
		//------------------------------------------


	}
	return;
}
//--------------------------------------------------------
void INT_TIM1_ISR(void) interrupt 3 //using 3//обработчик внешнего прерывани€ 2-использует внешний вход таймера T1
{
		unsigned char cnt;
			
		TH1=0xFF;
		TL1=0xFF;
	
		cnt=(frequency[FRQ_CHNL_1].time_counter>>SHEAR)&0x3;
	
		frequency[FRQ_CHNL_1].frame[cnt].event_counter++;
		frequency[FRQ_CHNL_1].frame[cnt].timestamp=frequency[FRQ_CHNL_1].time_counter;
	return;
}
//--------------------------------------------------------
void Timer2_ISR(void) interrupt 5 //using 3//обработчик прерывани€ счетного таймера частоты
{

	if(channels.I2C_CHNL.channels.frequency_modific==CHNL_FREQ_256) //измерение низкой частоты
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
			}
			break;
	
			case FRAME_TIME_2:
			{
				frequency[FRQ_CHNL_1].frame[1].time_copy=frequency[FRQ_CHNL_1].frame[1].timestamp-frequency[FRQ_CHNL_1].frame[0].timestamp;
				frequency[FRQ_CHNL_1].frame[1].event_copy=frequency[FRQ_CHNL_1].frame[1].event_counter;	
	
				frequency[FRQ_CHNL_1].frame[0].event_counter=0;
				//----------------------------------------------
			}
			break;
	
			case FRAME_TIME_3:
			{
				frequency[FRQ_CHNL_1].frame[2].time_copy=frequency[FRQ_CHNL_1].frame[2].timestamp-frequency[FRQ_CHNL_1].frame[1].timestamp;
				frequency[FRQ_CHNL_1].frame[2].event_copy=frequency[FRQ_CHNL_1].frame[2].event_counter;	
	
				frequency[FRQ_CHNL_1].frame[1].event_counter=0;
				//----------------------------------------------------
			}
			break;
	
			case FRAME_TIME_4:
			{
				frequency[FRQ_CHNL_1].frame[3].time_copy=frequency[FRQ_CHNL_1].frame[3].timestamp-frequency[FRQ_CHNL_1].frame[2].timestamp;
				frequency[FRQ_CHNL_1].frame[3].event_copy=frequency[FRQ_CHNL_1].frame[3].event_counter;	
	
				frequency[FRQ_CHNL_1].frame[2].event_counter=0;	
				//-----------------------------------------------------			
				frequency[FRQ_CHNL_1].time_counter=0;
			}
			break;
		}
	
		frequency[FRQ_CHNL_1].time_counter++;
		frequency[FRQ_CHNL_1].time_counter&=FRAME_TIME_4;
	}
	else //измерение высокой частоты
	{
		char i=0;
		char j=0;

		TF2=0;
	 	if(cycl_kanal<19)
		{
	  		cycl_kanal++;
		}
	 	else
	  	{
			 temp_Hz_kanal_mgnov=((unsigned long)TH1*0x100)|TL1;
			 TH1=TL1=0;
			 handle_freq_flag=1;
	  	}	
	}
	return;
}
//------------------------------------------------------------
#pragma OT(0,Speed)
void Frequency_Measure_Process(void)
 { 
 	static unsigned char frq_chnl=0;
	static unsigned long temp_freq=0; 

	if(channels.I2C_CHNL.channels.frequency_modific==CHNL_FREQ_256) //измерение низкой частоты
	{

		temp_freq=(((unsigned long)frequency[frq_chnl].frame[0].event_copy+(unsigned long)frequency[frq_chnl].frame[1].event_copy+(unsigned long)frequency[frq_chnl].frame[2].event_copy+(unsigned long)frequency[frq_chnl].frame[3].event_copy)<<18)/((frequency[frq_chnl].frame[0].time_copy+frequency[frq_chnl].frame[1].time_copy+frequency[frq_chnl].frame[2].time_copy+frequency[frq_chnl].frame[3].time_copy));

		if(temp_freq<=0xFFFF)
		{
			channels.frequency_buf=temp_freq;
		}
		else
		{
			channels.frequency_buf=0xFFFF;
		} 
	}
	else //измерение высокой частоты
	{
		if(handle_freq_flag==0)
		{
			return;
		}

		handle_freq_flag=0;

		poschet_intervalov++;
	
		 

		 TH1=TL1=0;
		 temp_Hz_kanal_sred+=temp_Hz_kanal_mgnov;
		 Hz_data_mgnov+=temp_Hz_kanal_mgnov;//*=10;
		 
		 if(poschet_intervalov==2)
		 {
			 Hz_data_mgnov*=5;

				if(Hz_data_mgnov<=0xFFFF)	//частота превышена
				{
					channels.frequency_buf=Hz_data_mgnov;
				}
				else
				{
					channels.frequency_buf=0xFFFF;
				}

			 Hz_data_mgnov=0;
		 }
	
	
	
	
		if(poschet_intervalov==2) // вычисл€етс€ среднее значение частоты за 10 сек
		{
				 poschet_intervalov=0;
			
			     if(sec_kanal_sred!=kol_sec_sred+1&&!(period_kanal_sred%(1000/Period_sred)))  
				 {
				 	  sec_kanal_sred++;
				 }
	
				 if(sec_kanal_sred==kol_sec_sred+1)
				 {
					  sym_kanal_sred-=Hz_kanal_sred[period_kanal_sred/(1000/Period_sred)][period_kanal_sred%(1000/Period_sred)];
					  sym_kanal_sred+=temp_Hz_kanal_sred;
					  Hz_data_sred=(float)sym_kanal_sred/(float)kol_sec_sred;


						if(Hz_data_sred<=0xFFFF) //частота превышена
						{
						 	channels.mid_frequency_buf=Hz_data_sred;
						}
						else
						{
							 channels.mid_frequency_buf=0xFFFF;
						}
				 }
				 else
				 {
				  		sym_kanal_sred=sym_kanal_sred+temp_Hz_kanal_sred;
				 }
				 
				 Hz_kanal_sred[period_kanal_sred/(1000/Period_sred)][period_kanal_sred%(1000/Period_sred)]=temp_Hz_kanal_sred;
			
				 if(period_kanal_sred<(kol_sec_sred*(1000/Period_sred))-1)	// подсчет периодов в 10 секундный интервал
				 {
				   	period_kanal_sred++;
				 }
				 else
				 {
				    period_kanal_sred=0;
				 }
	
				 temp_Hz_kanal_sred=0;
	
		}
	    cycl_kanal=0;
	}
 }