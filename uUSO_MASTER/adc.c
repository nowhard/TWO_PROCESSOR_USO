#include "adc.h"
//-------------------global variables-----------------------------------

sbit LED1=P3^7;
sbit LED2=P3^6;
volatile struct ADC_Channels xdata adc_channels[ADC_CHANNELS_NUM];

volatile unsigned char xdata mean_counter=0;

unsigned long tst=12345678;
//----------------------------------------------------------------------
/*void ADC_Calibrate(char mode) //using 0
{	
	volatile unsigned char xdata adc_mode=mode;
	ADC_Stop();
	ADCMODE &= 0xF8; // 1111 1000
	ADCMODE |= adc_mode;
	ADC_Run();
	while(!CAL) 
	{}
	ADC_Stop();
} */
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
/*void ADC_Run()	//using 0
{
	ADCMODE |= 0x20; //0010 0000 //ENABLE
	return;
}
//-------------------------------------------
void ADC_Stop()	 // using 0
{
	ADCMODE &= 0xDF; // 1101 1111
	return;
}*/
//-------------------------------------------
void ADC_ISR(void) interrupt 6 //using 1
{

//	EA=0;
	
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[0]=0x0;//получим результат
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[1]=ADC0H;
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[2]=ADC0M;
	adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_CHAR[3]=ADC0L; 
	
	//adc_channels[ADC0CON2&0x7].ADC_BUF_UN[adc_channels[ADC0CON2&0x7].adc_buf_counter].ADC_LONG=tst--;	//debug

	adc_channels[ADC0CON2&0x7].adc_buf_counter=(adc_channels[ADC0CON2&0x7].adc_buf_counter+1)&(ADC_BUF_SIZE-1);	//инкрементируем указатель усредняющего буфера текущего канала
	adc_channels[ADC0CON2&0x7].new_measuring=1;	 //новое измерение было

		
	ADCMODE &= 0xDF; // 1101 1111
	ADC0CON2=((ADC0CON2+1)&0x7)|(ADC0CON2&0xF0); //инкремент аналогового входа 	
	ADC0CON1=(ADC0CON1&0xF8)|((channels[ADC0CON2&0x7].settings.set.state_byte_1^0x7)&0x7);//восстанавливаем усиление следующего канала
	SF=channels[(ADC0CON2/*+1*/)&0x7].settings.set.state_byte_2;						 
	ADCMODE |= 0x20; //0010 0000 //ENABLE

	RDY0=0;


//	EA=1;
	return;
}

//--------------------------------------------------
/*void ADC_Set_Range(unsigned char range)	 //using 0
{
	ADC0CON1&=~0x7;

	ADC0CON1|=(range&0x7);
	return;
} */
//--------------------------------------------------
void ADC_Set_Mode(unsigned char mode)	//using 0
{
	ADCMODE&=~0x7;
	ADCMODE|=mode;
	return;
}
//--------------------------------------------------
/*void ADC_Set_Input(unsigned char input)	 //using 0
{
	ADC0CON2&=~0xF;		
	ADC0CON2 |=input;
	return;
}*/
//--------------------------------------------------
void ADC_Out_Freq(unsigned int freq)  // using 0
{
	SF=(unsigned char)(32768/3/8/freq);		  //частота выхода sinc3 фильтра (децимация)
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
 /*unsigned long Meaning( unsigned long mas[],unsigned char size) //using 2//находим матожидание по циклическому массиву усреднения
 {
 	 unsigned char xdata i=0,max=0,min=0;
	 unsigned long xdata result=0, fast_res=0;
	 unsigned long xdata delta=0;
	 unsigned long xdata *data_mas;
	 unsigned char xdata buf_size;
	 
 //debug

   	data_mas=mas;
   	buf_size=size;
	//------------------------------
	//если следующая величина больше предыдущей на 50%, заполняем массив этой величиной, улучшим быстродействие
	fast_res=data_mas[(adc_channels[mean_counter].adc_buf_counter-1)&(ADC_BUF_SIZE-1)];
	delta=labs(data_mas[(adc_channels[mean_counter].adc_buf_counter-1)&(ADC_BUF_SIZE-1)]-data_mas[(adc_channels[mean_counter].adc_buf_counter-2)&(ADC_BUF_SIZE-1)]) ;
	if(delta>(fast_res>>3))
	{
		for(i=0;i<ADC_BUF_SIZE;i++)
		{
			data_mas[i]=fast_res	;
		}

		return fast_res; 
	}		 
   //-----------------------------------

	for(i=1;i<buf_size;i++)				 //нужна серьезная оптимизация
	{
		
		if(!adc_channels[mean_counter].new_measuring)//если прерывание сдвинуло очередь, то начнем сначала
		{
			i=1;
			max=0;
			min=0;
		}
			

	   	if(data_mas[i]>data_mas[max])
		{
			max=i;
			continue;
		}

		//if(data_mas[i]<data_mas[min])
		 data_mas[i]<data_mas[min]? min=i:0;   //??????????? ???...(((
						
	}	  
	 
	//------------------------------------------
	if(max==min)					  
	{ 
		result=data_mas[max];	
	} 
	else
	{	
		for(i=0;i<buf_size;i++)
		{
				if(i!=max && i!=min)
				{
					result+=data_mas[i];
				}
		}							
		result=(unsigned long)(result/((buf_size-2)));		
	}
	return result;	   
 }*/
 //-----------------------------------------------------------------------------------------------------------------
 /*unsigned char Meaning_Process(void) //using 2//циклический процесс, получающий среднее значение массивов
 {
	LED1=~LED1;
	Get_Mid();
 	setTimer(ADC_MEAN_TASK, 0, 1);
 	return 0;
 }*/
 //-----------------------------------------------------------------------------------------------------------------
 