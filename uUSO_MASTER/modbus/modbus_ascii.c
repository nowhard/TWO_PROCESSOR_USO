#include "modbus_ascii.h"
#include <intrins.h>
#include "proto_uso/channels.h"
//-----------------------------------------------------------------------------------
extern volatile unsigned char xdata ADRESS_DEV;

extern volatile unsigned char xdata  RecieveBuf[]; //буфер принимаемых данных
extern volatile unsigned char xdata  TransferBuf[]; //буфер передаваемых данных

 extern struct Channel xdata channels[CHANNEL_NUMBER];//обобщенная структура каналов
//-----------------------LRC----------------------------------------------------------------------
unsigned char LRC_Check(unsigned char * pucFrame, unsigned int usLen )//
{
    unsigned char   ucLRC=0;  /* LRC char initialized */

    while( usLen )	  
    {
        ucLRC += (MBCHAR2BIN(*pucFrame++)<<4|MBCHAR2BIN(*pucFrame++))&0xFF;   /* Add buffer byte without carry */
		ucLRC&=0xFF;
		usLen-=2;
    }
    /* Return twos complement */
	ucLRC=((ucLRC^0xFF)+1)&0xFF;

    return ucLRC;	
}
//-------------------------------------------------------------------------------------------------------
unsigned char MBCHAR2BIN( unsigned char ucCharacter )//
{
    if( ( ucCharacter >= '0' ) && ( ucCharacter <= '9' ) )
    {
        return ( unsigned char )( ucCharacter - '0' );
    }
    else if( ( ucCharacter >= 'A' ) && ( ucCharacter <= 'F' ) )
    {
        return ( unsigned char )( ucCharacter - 'A' + 0x0A );
    }
    else
    {
        return 0xFF;
    }
}
//----------------------------------------------------------------------------------------------
unsigned char MBBIN2CHAR( unsigned char ucByte ) //
{
    if( ucByte <= 0x09 )
    {
        return ( unsigned char )( '0' + ucByte );
    }
    else if( ( ucByte >= 0x0A ) && ( ucByte <= 0x0F ) )
    {
        return ( unsigned char )( ucByte - 0x0A + 'A' );
    }
    else
    {
 		return 0xFF;
    }
    return '0';
}
//-----------------------------------------------------------------------------------------------
unsigned int Sym_4_To_Int(unsigned char * pucFrame)//
{
	union
	{
		unsigned int result_int;
		unsigned char result_char[2];
	}
	sym_4_to_int;
	sym_4_to_int.result_char[0]=(unsigned char)MBCHAR2BIN(pucFrame[0])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[1]);
	sym_4_to_int.result_char[1]=(unsigned char)MBCHAR2BIN(pucFrame[2])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[3]);
	
	return sym_4_to_int.result_int;	
}
//-----------------------------------------------------------------------------------------------
unsigned char Sym_2_To_Char(unsigned char * pucFrame)//
{
	unsigned char result_char;

	result_char= (unsigned char)MBCHAR2BIN(pucFrame[0])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[1]);
	return result_char; 
}
//-----------------------------------------------------------------------------------------------
void Int_To_Sym_4(unsigned int val, unsigned char* buf)//
{
	union
	{
		unsigned int int_num;
		unsigned char result_char[2];
	}
	int_to_sym_4;
	
	int_to_sym_4.int_num=val;

	buf[1]=MBBIN2CHAR(int_to_sym_4.result_char[0]&0xF);
	buf[0]=MBBIN2CHAR((int_to_sym_4.result_char[0]>>4)&0xF);
	buf[3]=MBBIN2CHAR(int_to_sym_4.result_char[1]&0xF);
	buf[2]=MBBIN2CHAR((int_to_sym_4.result_char[1]>>4)&0xF);

	return;
}
//-----------------------------------------------------------------------------------------------
void Char_To_Sym_2(unsigned char val, unsigned char* buf)//
{	
	buf[0]=(unsigned char)MBBIN2CHAR( (unsigned char)((val>>4)&0xF));
	buf[1]=(unsigned char)MBBIN2CHAR( (unsigned char)(val&0xF));	
}
//-----------------------------------------------------------------------------------------------
unsigned long Sym_8_To_Long(unsigned char * pucFrame)//
{
	union
	{
		unsigned long result_long;
		unsigned char result_char[4];
	}
	sym_8_to_long;

	sym_8_to_long.result_char[0]=(unsigned char)MBCHAR2BIN(pucFrame[0])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[1]);
	sym_8_to_long.result_char[1]=(unsigned char)MBCHAR2BIN(pucFrame[2])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[3]);
	sym_8_to_long.result_char[2]=(unsigned char)MBCHAR2BIN(pucFrame[4])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[5]);
	sym_8_to_long.result_char[3]=(unsigned char)MBCHAR2BIN(pucFrame[6])<<4 | (unsigned char)MBCHAR2BIN(pucFrame[7]);	

	return sym_8_to_long.result_long;			
}
//-----------------------------------------------------------------------------------------------
void Long_To_Sym_8(unsigned long val, unsigned char* buf)//
{
   unsigned char i;

	for(i=0;i<8;i++)
	{
		buf[i]=MBBIN2CHAR( (unsigned char)((val&0xF0000000)>>28));
		val=val<<4;	
	}
}

//-----------------------------------------------------------------------------
//unsigned char SetSingleRegister(void)
//{
//	static unsigned int   addr;
//	static unsigned int   val;
//	static unsigned char i;
//
//	addr=Sym_4_To_Int(&RecieveBuf[5]);
//	num=Sym_4_To_Int(&RecieveBuf[9]);
//
//	if(addr>=REG_ADDR_MIN && addr<=REG_ADDR_MAX)
//	{
//		//controller_reg[addr]=val;	//устанавливаем значение регистра
//	}
//	else
//	{
//		return 0;
//	}
//
//	for(i=0;i<buf_count;i++)
//	{
//		TransferBuf[i]=	RecieveBuf[i];	
//	}
//
//	return buf_count;//
//	
//	return;
//}
//-----------------------------------------------------------------------------
unsigned char ReadHoldingReg(void)
{
  #define HEAD_LEN	7
//[addr_sym_1|addr_sym_2|func_sym_1|func_sym_2|1st|2st|3st|4st|1n|2n|3n|4n|lrc1|lrc2]
//[		head	   |............]	
	static unsigned char  i=0,count=0;
	static unsigned char  LRC=0x0;
	static unsigned int   addr;
	static unsigned int   len;
	
	count=0;

	addr=Sym_4_To_Int(&RecieveBuf[3]);
	len= Sym_4_To_Int(&RecieveBuf[5]);

	if(addr>=REG_ADDR_MIN && addr<=(REG_ADDR_MIN+CHANNEL_NUMBER) && len<=CHANNEL_NUMBER)  
	{			
		TransferBuf[0]=':';
		count++;

		Char_To_Sym_2(ADRESS_DEV,&TransferBuf[1]);
		count+=2;

		Char_To_Sym_2(MB_FUNC_READ_HOLDING_REGISTER,&TransferBuf[3]);
		count+=2;

		//----
		count+=2;//счетчик байт, вставим после вставки данных
		//----

		for(i=addr;i<((addr+len)>>1);i++) //в устройстве все регистры 4-байтовые
		{
			Long_To_Sym_8(channels[i].channel_data,&TransferBuf[count]);
			count+=8;
		}		

		Char_To_Sym_2(count-HEAD_LEN,&TransferBuf[5]);
		
		LRC=LRC_Check(&TransferBuf[1],(count-1));
		
		Char_To_Sym_2(LRC,&TransferBuf[count]);
		count+=2;
		
		TransferBuf[count]='\r';
		TransferBuf[count]='\n';
		count+=2;							                     
	}
	else
	{
		return 0;
	}

	return count;
}
//-----------------------------------------------------------------------------------
