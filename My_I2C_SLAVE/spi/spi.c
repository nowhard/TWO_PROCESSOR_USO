#include "spi.h"
//---------------------------------------------------------------------------------
void SPI_Transfer_Byte(unsigned char spi_data)//�������� ���� �� SPI
{
	SPIDAT=spi_data;
}
//---------------------------------------------------------------------------------
void SPI_Init(unsigned char mode,unsigned char polarity,unsigned char edge,unsigned char rate)//������������� SPI ����������
{
	SPICON|=(mode|polarity|edge|rate);
	SPE=1;//SPI enable
}
//---------------------------------------------------------------------------------
