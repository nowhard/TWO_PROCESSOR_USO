#ifndef LED_PROCESS_H
#define LED_PROCESS_H
#include <ADuC845.h>
sbit CS=P0^5;			   //каналы spi bit-bang
sbit WR_DATA=P3^7;
sbit CLK=P3^6;

#define LED_DIG_NUM	5

//состояния автомата передачи данных bit-bang
//-------------------------------------------
#define SPI_WAIT		1//состояние ожидания автомата
#define	SPI_SEND_WORD	2//посылка 16-разрядного слова в драйвер-led
#define SPI_SEND_STROBE	3//послать строб захвата в драйвер
#define SPI_DELAY		4//задержка между тактами
#define SPI_INT_TO_BCD	5//перевод целого в двоично-десятичный

//-------------------------------------------

void LED_Process(void);//главный процесс вывода
void LED_Out_Buf(unsigned int out_buf[], unsigned char out_buf_len);//вывод буфера в SPI bit-bang

void LED_Out_Integer(unsigned int num);//вывод целочисленного значения
void LED_Out_Float(float num);//вывод значения с плавающей точкой

#endif