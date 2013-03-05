#include "adc.h"
//-------------------global variables-----------------------------------
volatile struct ADC_Channels xdata adc_channels[ADC_CHANNELS_NUM];
#pragma OT(6,Speed)
//-------------------------------------------
void ADC_Initialize() //using 0
{	
	ADCMODE&=0x0;
	ADC0CON1&=0x0;	
    ADC0CON2&=0x0;
	//ADCMODE=0xB;
//	ADCMODE=0x3; //chop enable

//	ADCMODE|=0x8;//CHOP DISABLE

    ADC0CON1|=BUF_BYPASS;
	ADC0CON1|=ADC_UNIPOLAR;
	ADC0CON1|=RN_2560;


	ADC_Restore_Settings();
	ADC_Set_Mode(ADC_CONTIN_CONV);

	ADC_Out_Freq(100);	

	ADC0CON2&=0xF0;
	ADC0CON2|=EXT_REF;//включим внешнюю опору
	EADC = 1;

	ADCMODE |= 0x20; //0010 0000 //ENABLE

	return;
}
//-------------------------------------------
void ADC_ISR(void) interrupt 6 //using 1
{
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[0]=0x0;//получим результат
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[1]=ADC0H;
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[2]=ADC0M;
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[3]=ADC0L; 

	adc_channels[ADC0CON2&0x7].adc_buf_counter=(adc_channels[ADC0CON2&0x7].adc_buf_counter+1)&(ADC_BUF_SIZE-1);	//инкрементируем указатель усредн€ющего буфера текущего канала
	adc_channels[ADC0CON2&0x7].new_measuring=1;	 //новое измерение было
		
	ADCMODE &= 0xDF; // 1101 1111
	ADC0CON2=((ADC0CON2+1)&0x7)|(ADC0CON2&0xF0); //инкремент аналогового входа 	
	ADC0CON1=(ADC0CON1&0xF8)|((channels[ADC0CON2&0x7].settings.set.state_byte_1^0x7)&0x7);//восстанавливаем усиление следующего канала
	SF=channels[(ADC0CON2/*+1*/)&0x7].settings.set.state_byte_2;						 
	ADCMODE |= 0x20; //0010 0000 //ENABLE

	RDY0=0;
//	return;
}
//--------------------------------------------------
void ADC_Set_Mode(unsigned char mode)	//using 0
{
	ADCMODE&=~0x7;
	ADCMODE|=mode;
	return;
}
//--------------------------------------------------
void ADC_Out_Freq(unsigned int freq)  // using 0
{
	SF=(unsigned char)(32768/3/8/freq);		  //частота выхода sinc3 фильтра (децимаци€)
	return;
}
//------------------------------------------------------------------------------------------------------------------
void ADC_Restore_Settings(void) //using 0//восстановление настроек ацп
{
	 unsigned char xdata i=0;
	for(i=0;i<ADC_CHANNELS_NUM;i++)
	{
		adc_channels[i].adc_buf_counter=0;
		adc_channels[i].buffered=BUF_ENABLE;
		adc_channels[i].unipolar=ADC_UNIPOLAR;
		adc_channels[i].new_measuring=0;	
	}
	return;
}
 //------------------------------------------------------------------------------------------------------------------
