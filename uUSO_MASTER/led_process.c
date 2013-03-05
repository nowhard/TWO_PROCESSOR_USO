#include "led_process.h"
//----------------------------------------------
volatile unsigned char spi_auto=SPI_WAIT;//������� �������� spi ��� led-�������� 
volatile unsigned char spi_last_state;//���������� ��������� 
volatile unsigned char spi_bit_counter;//������� ������������ ����� ��� bit-bang
volatile unsigned int spi_data;//������ bit-bang

volatile unsigned char spi_delay;//�������� ����� �������
volatile unsigned int spi_buf[16]={0xC01,0x9FF,0xF00,0xA0E,0xB05};
volatile unsigned char spi_buf_length;
volatile unsigned char spi_buf_counter;

volatile unsigned char cs_auto=0;
volatile unsigned char sw_auto=0;
volatile unsigned char num_conv_counter=0;

volatile unsigned int integer_num=0;
volatile float float_num=0;
//----------------------------------------------
void LED_Process(void)//������� ������� ������
{
	switch(spi_auto)
	{
//------------------------------------------------------------------
		case SPI_WAIT:
		{
		//�����	
		}
		break;
//------------------------------------------------------------------
 		case SPI_SEND_WORD:	//������� �����
		{
			switch(sw_auto)
			{
				case 0:
				{
					spi_data=spi_data<<1;//
					WR_DATA=CY;
					CLK=1;
					spi_bit_counter++;
					spi_last_state=spi_auto;//��������� ���������
					spi_auto=SPI_DELAY;
					spi_delay=1;//
					sw_auto=1;	
				}
				break;

				case 1:
				{
					CLK=0;
					spi_last_state=spi_auto;//��������� ���������
					spi_auto=SPI_DELAY;
					spi_delay=1;//
					
					if(spi_bit_counter>=16)
					{
						sw_auto=2;
					}
					else
					{
						sw_auto=0;
					}	
				}
				break;

				case 2:
				{
					spi_auto=SPI_SEND_STROBE ;//
					spi_bit_counter=0;
					cs_auto=0;	
					sw_auto=0;
				}
				break;
			}	
		}
		break;
//------------------------------------------------------------------
		case SPI_SEND_STROBE:
		{
			switch(cs_auto)
			{
				case 0:
				{
					CS=1;
					cs_auto=1;
				}
				break;

				case 1:
				{
					CS=0;
					if(spi_buf_counter<spi_buf_length)
					{
						spi_data=spi_buf[spi_buf_counter];
						spi_buf_counter++;
						//spi_bit_counter=0;
						spi_auto=SPI_SEND_WORD;
					}
					else
					{
						spi_auto=SPI_WAIT;
					}
					cs_auto=0;					
				}
				break;
			}
		}
		break;
//------------------------------------------------------------------
		case SPI_DELAY:	 //�������� ����� �������
		{
		   spi_delay--;
		   if(!spi_delay)
		   {
		   		spi_auto=spi_last_state;			
		   }
		}
		break;
//------------------------------------------------------------------
		case SPI_INT_TO_BCD: //������� ������ � �������-����������
		{
			if(num_conv_counter<LED_DIG_NUM) //��������� � bcd
			{
				spi_buf[num_conv_counter+5]=integer_num%10|(0x100*(LED_DIG_NUM-num_conv_counter));
				integer_num=integer_num/10;	
				num_conv_counter++;
			}
			else
			{
				spi_buf_counter=0;
				spi_data=spi_buf[spi_buf_counter];
				spi_buf_counter++;
				spi_buf_length=5+LED_DIG_NUM;
				CS=0;
				spi_auto=SPI_SEND_WORD;
				sw_auto=0;
				num_conv_counter=0;	
			}
		}
		break;
//------------------------------------------------------------------
		default:
		{
			cs_auto=0;
			sw_auto=0;
			num_conv_counter=0;
			CS=0;
			spi_auto=SPI_WAIT;
		}
		break;
	}
	return;
}

//--------------------------------------------------------------------------------
void LED_Out_Float(float num)//����� �������� � ��������� ������  //�� �����������
{

	float_num=num;

	return;
}
//--------------------------------------------------------------------------------
void LED_Out_Integer(unsigned int num)//����� �������������� ��������
{
	if(spi_auto==SPI_WAIT)
	{
		integer_num=num;//...
		spi_auto=SPI_INT_TO_BCD;
	}
	return;
}
//--------------------------------------------------------------------------------
void LED_Out_Buf(unsigned int out_buf[], unsigned char out_buf_len)//����� ������ � SPI bit-bang
{
	unsigned char i;

	if(spi_auto!=SPI_WAIT)
	{
		return;
	}
	//��������� � ���� �����
	for(i=0;i<(out_buf_len&0xF);i++)
	{
		spi_buf[i]=out_buf[i];	
	}
	
	spi_buf_counter=0;
	spi_data=spi_buf[spi_buf_counter];
	spi_buf_counter++;
	spi_buf_length=out_buf_len;
	CS=0;
	spi_auto=SPI_SEND_WORD;
	sw_auto=0;
	return;	
}