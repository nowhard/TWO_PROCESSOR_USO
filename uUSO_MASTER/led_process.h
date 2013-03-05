#ifndef LED_PROCESS_H
#define LED_PROCESS_H
#include <ADuC845.h>
sbit CS=P0^5;			   //������ spi bit-bang
sbit WR_DATA=P3^7;
sbit CLK=P3^6;

#define LED_DIG_NUM	5

//��������� �������� �������� ������ bit-bang
//-------------------------------------------
#define SPI_WAIT		1//��������� �������� ��������
#define	SPI_SEND_WORD	2//������� 16-���������� ����� � �������-led
#define SPI_SEND_STROBE	3//������� ����� ������� � �������
#define SPI_DELAY		4//�������� ����� �������
#define SPI_INT_TO_BCD	5//������� ������ � �������-����������

//-------------------------------------------

void LED_Process(void);//������� ������� ������
void LED_Out_Buf(unsigned int out_buf[], unsigned char out_buf_len);//����� ������ � SPI bit-bang

void LED_Out_Integer(unsigned int num);//����� �������������� ��������
void LED_Out_Float(float num);//����� �������� � ��������� ������

#endif