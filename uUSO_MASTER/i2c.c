#include "i2c.h"
#include "crc_table.h"//для crc
//-------------------------------------------------------------

volatile unsigned char xdata DEV_ADDR;

volatile unsigned char xdata byte_num_write=0;//количество байт в массиве
volatile unsigned char xdata byte_num_read=0;//количество байт в массиве(при вторичном старте)
volatile unsigned char *i2c_buf;//

//volatile unsigned char xdata repeated_strt_read=0, i2c_address_write=0;
volatile unsigned char  temp_val=0x12;
volatile unsigned char i2c_mas[10]={0};


volatile struct I2C_Channel xdata i2c_channels;

unsigned char idata i2c_buffer[6]={0};

extern volatile unsigned char xdata  STATE_BYTE;


//-------------------------------------------------------------
volatile unsigned char START_I2C=0;

volatile unsigned char ERROR_I2C=0;

struct pt pt_i2c_read_buf, pt_i2c_write_buf,pt_i2c_read_complete;

static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len));//дочерний поток чтения буфера I2C
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len));//дочерний поток записи буфера I2C
static PT_THREAD(I2C_Read_Complete(struct pt *pt));//постобработка пакета
//-------------------------------------------------------------

//-------------------------------------------------------------

void I2C_Init(void)
{
	I2CM=1;//ведущий
	PT_INIT(&pt_i2c_read_buf);//инициализация дочерних потоков
	PT_INIT(&pt_i2c_write_buf);
	PT_INIT(&pt_i2c_read_complete);

	return;
}
//--------------------------------------------------------------
void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len)//запрос с двумя стартами
{


	DEV_ADDR=(addr<<1)&0xFE;

	
	byte_num_write=par_buf_len;
	byte_num_read=buf_len;
	
	i2c_buf=par_buf;
	//repeated_strt_read=1;
	START_I2C=1;//стартуем
	
	return;
}
//-------------------------------------------------------------
//#pragma OT(0,Speed) 
 PT_THREAD(I2C_Process(struct pt *pt))//поток  I2C
 {  
   PT_BEGIN(pt);

  while(1) 
  {
	 
	 PT_WAIT_UNTIL(pt,START_I2C);
	 START_I2C=0;

//	 //--------------start---------------
	 MDE=1;	//software output enable


	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	  			   
		 PT_YIELD(pt);//дадим другим процессам время	   
		 MDO=0;				   
	   //------------------------

	 //-------------передача буфера-------

	 PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1)); //передаем адрес


	  if(ERROR_I2C)//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;				
			PT_YIELD(pt);//дадим другим процессам время										
			MDO=1;
		  //------------------------
		    ERROR_I2C=0;			
			PT_RESTART(pt);	
	  }
	  //PT_YIELD(pt);//дадим другим процессам время
  //--------------------------------------------------------------------------------------------------
	  
	  //i2c_buf[0]=0x8;

	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&i2c_buffer/*i2c_buf*/,byte_num_write)); //передаем параметры
	  if(ERROR_I2C)//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;				
			PT_YIELD(pt);//дадим другим процессам время										
			MDO=1;
		  //------------------------
		  	ERROR_I2C=0;	  		
			PT_RESTART(pt);	
	  }
	  //PT_YIELD(pt);//дадим другим процессам время
////------------------------------------------------------------------------------------------------------
	  //-----------повторный старт--------
	   //-------I2C START--------
	   	 MCO=0;
	     MDO=1;	   			   
		 MCO=1;	   			   
		 PT_YIELD(pt);//дадим другим процессам время	   
		 MDO=0;				   
	   //------------------------
	  DEV_ADDR|=1;


	  PT_SPAWN(pt, &pt_i2c_write_buf, I2C_Write_Buf(&pt_i2c_write_buf,&DEV_ADDR,1)); //передаем адрес

	  if(ERROR_I2C)//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;	
			PT_YIELD(pt);//дадим другим процессам время										
			MDO=1;
		  //------------------------
		    ERROR_I2C=0;	  	
			PT_RESTART(pt);	
	  }
	 // PT_YIELD(pt);//дадим другим процессам время
 //---------------------------------------------------------------------------------------------------------------------
	  PT_SPAWN(pt, &pt_i2c_read_buf, I2C_Read_Buf(&pt_i2c_read_buf,&i2c_channels.I2C_CHNL.i2c_buf,byte_num_read)); //принимаем данные
	  if(ERROR_I2C)//смотрим ошибки
	  {
		  //-------I2C STOP--------
		  	MCO=0;
		    MDO=0;	  				
			MCO=1;					
			PT_YIELD(pt);//дадим другим процессам время									
			MDO=1;
		  //------------------------
		  	ERROR_I2C=0;	  		
			PT_RESTART(pt);	
	  }
	 // PT_YIELD(pt);//дадим другим процессам время

	  //-------I2C STOP--------
		MCO=0;
	    MDO=0;	  				
		MCO=1;
		PT_YIELD(pt);//дадим другим процессам время									
		MDO=1;

	  //-----------------------------------------------------------------------------

	  PT_SPAWN(pt, &pt_i2c_read_complete, I2C_Read_Complete(&pt_i2c_read_complete));

	 //----------------------------------
  }
  PT_END(pt);

 }
//-------------------------------------------------------------
//#pragma OT(0,Speed)
 static PT_THREAD(I2C_Read_Buf(struct pt *pt,unsigned char *buf,unsigned char len))//дочерний поток чтения буфера I2C
 {  
	    static volatile unsigned char read_byte_counter,bit_counter,recieve_byte;
	   
	  PT_BEGIN(pt);
	  
	  read_byte_counter=0;

	  while(read_byte_counter<len) //цикл приема буфера
	  {	
			recieve_byte=0;
			MDE=0;//прием 
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			for(bit_counter=0;bit_counter<8;bit_counter++) //цикл приема байта
			{
				MCO=1;

				//PT_YIELD(pt);//дадим другим процессам время
				recieve_byte=(recieve_byte<<1)|MDI;
				MCO=0;
			} //приняли байт
			
			buf[read_byte_counter]=recieve_byte;
			read_byte_counter++;


			MDE=1;//на передачу и отсылаем ACK
			if(read_byte_counter<len) //ack
			{
					//ACK	
				MDO=0;

			}
			else  //nack
			{
				//NACK		
				MDO=1;			
			}
				
			MCO=1;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=0;
				
			MDE=0;//прием

	  }
	  MDE=1;
	  ERROR_I2C=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
//#pragma OT(0,Speed)
static PT_THREAD(I2C_Write_Buf(struct pt *pt,unsigned char *buf,unsigned char len))//дочерний поток записи буфера I2C
 {  
	   static volatile unsigned char write_byte_counter,bit_counter,write_byte;
	  PT_BEGIN(pt);
	  MDE=1;//передача 
	  write_byte_counter=0;
	  while(write_byte_counter<len)  //цикл передачи буфера
	  {
	  	//	bit_counter=0;

			write_byte=buf[write_byte_counter];
			write_byte_counter++;
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			for(bit_counter=0;bit_counter<8;bit_counter++)
			{
				
				write_byte=write_byte<<1;//>>1;	 //???
				MDO=CY;
			
				MCO=1;
				PT_YIELD(pt);//дадим другим процессам время
				MCO=0;
//				PT_YIELD(pt);//дадим другим процессам время

			}
			MDE=0;//прием 
			MCO=0;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=1;
			PT_YIELD(pt);//дадим другим процессам время
			MCO=0;	
			MDE=1;//передача
			if(MDI) //NACK
			{
				ERROR_I2C=1;//случилась ошибка	
				PT_EXIT(pt);
			}
	  }
	  ERROR_I2C=0;
	  PT_END(pt);
}
//-------------------------------------------------------------
//#pragma OT(0,Speed)
static PT_THREAD(I2C_Read_Complete(struct pt *pt))//постобработка пакета
{
	PT_BEGIN(pt);
   if(i2c_channels.I2C_CHNL.channels.CRC==CRC_Check(i2c_channels.I2C_CHNL.i2c_buf,9))
   {
 	channels[11].channel_data=i2c_channels.I2C_CHNL.channels.DOL;
	channels[12].channel_data=i2c_channels.I2C_CHNL.channels.frequency;
	channels[13].channel_data=i2c_channels.I2C_CHNL.channels.mid_frequency;
	STATE_BYTE|=i2c_channels.I2C_CHNL.channels.state_byte;//обновляем байт состояния
   }
   i2c_buffer[0]=0;//не сбрасывать флаг инициализации
	
	PT_END(pt);	
}