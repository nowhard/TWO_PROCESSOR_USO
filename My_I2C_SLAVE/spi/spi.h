#include <ADuC845.h>
//----------------------------------------------------
#define CPOL_HIGH  0x8 //���������� ������� -/\-
#define CPOL_LOW   0 //���������� ������� -\/-
#define CPHA_LEAD  4 //�� ��������� ������
#define CPHA_TRAIL 0 //�� ������� ������

#define SPI_MASTER	0x10//master
#define SPI_SLAVE	0x0 //slave	
//-------------------spi rate-------------------------
#define SPI_BITRATE_F_DIV_2  0 //�������� �������� ������ �� SPI �� ������� ����
#define SPI_BITRATE_F_DIV_4  1
#define SPI_BITRATE_F_DIV_8  2
#define SPI_BITRATE_F_DIV_16 3
//----------------------------------------------------
void SPI_Transfer_Byte(unsigned char spi_data);//�������� ���� �� SPI
void SPI_Init(unsigned char mode,unsigned char polarity,unsigned char edge,unsigned char rate);//������������� SPI ����������
