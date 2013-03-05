#include "timer2.h"
//-------------------------------------------------------
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
//-------------------------------------------------------
void Timer2_Initialize(void) //using 0// инициализаци€ таймера  таймера дл€ скоростного частотомера
{
    ET2 = 1;
	PT2=0;//приоритет низкий
	T2CON=0x00;			  // таймер периода сбора частоты (0.01)
	RCAP2H=TH2=0x86;//0xC3;//0x86;//0x0B;	- дл€ 6,29 ћ√ц  //0x0A 
	RCAP2L=TL2=0x07;//0x03;//0x07;//0xF0; - дл€ 6,29 ћ√ц   //0x3C 
	TR2=1;
	return;
}
//-------------------------------------------------------
void _TR2_ (void) interrupt 5
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

//----------------------------------------------------------------------------------
void Frequency_Handler(void)//обработчик измерени€ частоты
{

		if(handle_freq_flag==0)
		{
			return;
		}

		handle_freq_flag=0;

		poschet_intervalov++;
	
		 
		// temp_Hz_kanal_mgnov=((unsigned long)TH1*0x100)|TL1;
		 TH1=TL1=0;
		 temp_Hz_kanal_sred+=temp_Hz_kanal_mgnov;
		 Hz_data_mgnov+=temp_Hz_kanal_mgnov;//*=10;
		 
		 if(poschet_intervalov==2)
		 {
			 Hz_data_mgnov*=5;
		 //	if(channels.transfer==0)	//если передачи нет
		//	{
		 		//channels.I2C_CHNL.channels.frequency=Hz_data_mgnov;//FloatToStrPC((void*)&Hz_data_mgnov);
				channels.frequency_buf=Hz_data_mgnov;
		//	}
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

				  //	if(channels.transfer==0)	//если передачи нет
				//	{
				  		//channels.I2C_CHNL.channels.mid_frequency=Hz_data_sred;//FloatToStrPC((void*)&Hz_data_sred);
						 channels.mid_frequency_buf=Hz_data_sred;
				//	}
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
	return;
}