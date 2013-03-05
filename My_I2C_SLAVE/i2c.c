#include "i2c.h"
//-------------------------------------------------------------
volatile struct I2C_STATE  xdata i2c_st;//��������� ���������
volatile unsigned char xdata delay_counter=0;//������� ��������
volatile unsigned char xdata DEV_ADDR;
volatile unsigned char  xdata transfer_byte=0;
volatile unsigned char xdata recieve_byte=0;
volatile unsigned char xdata recieve_byte_counter=0;
volatile unsigned char xdata transfer_byte_counter=0;
volatile unsigned char xdata bit_counter=0;
volatile unsigned char xdata byte_num=0;//���������� ���� � �������
volatile unsigned char xdata byte_num2=0;//���������� ���� � �������(��� ��������� ������)
volatile unsigned char *i2c_buf;//
volatile unsigned char *i2c_buf2;//������ ������������ ��� ������� ������
volatile unsigned char xdata rec_tr_flag=0;//�������� ��� �����
volatile unsigned char xdata second_start=0;
volatile unsigned char xdata repeated_strt_read=0;
volatile unsigned char xdata data_transfered_flag=0,byte_transfered_flag=0;

volatile unsigned char xdata operation_status=0;//������ ��������-0-���������, 1 �� ���������

volatile unsigned char xdata op_flag=0;

volatile unsigned char i2c_mas[10];
//-------------------------------------------------------------

void I2C_Init(void)
{
	I2CM=1;//�������
	i2c_st.GLOBAL_STATE=I2C_RESTART;

	setHandler(I2C_IS_WRITE_MSG , &I2C_Is_Write);
	setHandler(I2C_IS_READ_MSG , &I2C_Is_Read);

	return;
}
//-------------------------------------------------------------
void I2C_Write_Buf(unsigned char addr,unsigned char *buf,unsigned char buf_len) //������ ������ � ����
{
	rec_tr_flag=0;//�� ��������
	byte_num=buf_len;
	i2c_buf=buf;
	transfer_byte=addr;//������ ������ �������� �����
	i2c_st.GLOBAL_STATE=I2C_START;
	return;	
}
//-------------------------------------------------------------
void I2C_Read_Buf(unsigned char addr,unsigned char *buf,unsigned char buf_len)  //������ �� ���� � �����
{
  	rec_tr_flag=1;//�� �����
	byte_num=buf_len;
	i2c_buf=buf;
	transfer_byte=addr|1;//0x80;//������ ������ �������� �����
	i2c_st.GLOBAL_STATE=I2C_START;
	return;
}
//-------------------------------------------------------------
void I2C_Repeat_Start_Read(unsigned char addr,unsigned char *par_buf,unsigned char par_buf_len,unsigned char *buf,unsigned char buf_len)//������ � ����� ��������
{
	repeated_strt_read=1;	//� ��������

	DEV_ADDR=addr;
	transfer_byte=addr;//������ ������ �������� �����

	rec_tr_flag=0;//�� ��������
	
	byte_num=par_buf_len;
	byte_num2=buf_len;
	
	i2c_buf=par_buf;
	i2c_buf2=buf;

	i2c_st.GLOBAL_STATE=I2C_START;
	return;
}
//-------------------------------------------------------------
void I2C_Process(void)	//������� �������� ���������
{
	switch(i2c_st.GLOBAL_STATE)
	{
//-------------------------------------------------------------
		case I2C_START:	   //full
		{
			MDE=1;	//software output enable
			//puts("I2C_START\n");
			switch(i2c_st.START_STATE) //�������� ����� �� ��������� ���������
			{
//--------------
				case 0:
				{
					MDO=1;	   // START
					MCO=1;	   //clock out

					//transfer_byte= DEV_ADDR;//	������ ��������� �����

					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.START_STATE=1;//
				}
				break;
//--------------
				case 1:
				{
					MDO=0;


					//transfer_byte= DEV_ADDR;//	������ ��������� �����

					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.START_STATE=2;//
				}
				break;
//--------------
				case 2:
				{
					
					MCO=0;	   //------
					i2c_st.START_STATE=0;//
					//if(!second_start) //��������� �����
					{
						i2c_st.GLOBAL_STATE=I2C_TRANSFER_BYTE;
						 //second_start=1;
					}
				/*	else
					{
						i2c_st.GLOBAL_STATE=I2C_RECIEVE_BYTE;
						//second_start=1;	
					}  */
					//debug
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					//debug
				}
				break;
//--------------				
			}
		}
		break;
 //-------------------------------------------------------------
		case I2C_STOP:
		{
		//	//puts("I2C_IN_STOP\n") ;
		
				MDE=1;	//software output enable
				//puts("I2C_STOP\n");
				switch(i2c_st.STOP_STATE) //�������� ����� �� ��������� ���������
				{
	//--------------
					case 0:
					{
						MDO=0;	  // STOP  ???
						MCO=0;
	
						i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
						delay_counter=1;
						i2c_st.GLOBAL_STATE=I2C_DELAY;
						i2c_st.STOP_STATE=1;//
					}
					break;
	//--------------
						case 1:
					{

						MCO=1;
						i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
						delay_counter=1;
						i2c_st.GLOBAL_STATE=I2C_DELAY;
						i2c_st.STOP_STATE=2;//
					}
					break;
	//--------------
					case 2:
					{	
						
						
						MDO=1;
								///???
							  //-----	   //------
						i2c_st.STOP_STATE=0;//
						i2c_st.GLOBAL_STATE=I2C_RESTART;//��������������� ���� ����������
						//������� ���������
						if(!operation_status)//���� �������� ���� ��������
						{
							if(!rec_tr_flag)//���� ��������, �� �������� ���������, ��� �������� 
							{
								sendMessage(I2C_IS_WRITE_MSG,0);
							}
							else //���� ���������, �������� ���������, ��� �������
							{
								sendMessage(I2C_IS_READ_MSG,0);
							}
						}
					}
					break;
	//--------------
				}
		}
		break;
//-------------------------------------------------------------
		case I2C_REPEAT_START:
		{
		   		//puts("I2C_REPEAT_START\n");
			  	repeated_strt_read=0;
				second_start=1;

				rec_tr_flag=1;//�� �����
				byte_num=byte_num2;
				i2c_buf=i2c_buf2;
				transfer_byte=DEV_ADDR|1;//0x80;//������ ������ �������� �����
				i2c_st.GLOBAL_STATE=I2C_START;

				recieve_byte_counter=0;
			 	transfer_byte_counter=0;
			 	bit_counter=0;
		}
		break;
//-------------------------------------------------------------
		case I2C_SET_ACK:  // full
		{
			MDE=1;	//software output enable
		//	//puts("I2C_SET_ACK\n");
			//--------------
			switch(i2c_st.SET_ACK_STATE) //��������  �� ��������� ���������
			{
				case 0:
				{
				//	MDE=1; // �������� ������������� ���������
					MDO=0;
					MCO=1;	  // 		

					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.SET_ACK_STATE=1;//
				}
				break;
//--------------
				case 1:
				{
					
					MCO=0;
					MDO=1;	  
					i2c_st.SET_ACK_STATE=0;//

					//if(rec_tr_flag)	//���� �����
					//{
					i2c_st.GLOBAL_STATE=I2C_RECIEVE_BYTE;//�������� ��������� ����
					//}

					//debug
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					//debug
				}
				break;
			}
		}
		break;
//-------------------------------------------------------------
		case I2C_SET_NACK:	  //full
		{
			MDE=1;	//software output enable
		//	//puts("I2C_SET_NACK\n");
//--------------
			switch(i2c_st.SET_NACK_STATE) //�������� ����� �� ��������� ���������
			{
				case 0:
				{
				//	MDE=1; // �������� jnhbw ���������
					MDO=1;	  // 		
					MCO=1;
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.SET_NACK_STATE=1;//
				}
				break;
//--------------
				case 1:
				{
					MCO=0;
					MDO=0;
						  
					i2c_st.SET_NACK_STATE=0;//

					//if(rec_tr_flag)	//���� �����
					//{
					i2c_st.GLOBAL_STATE=I2C_STOP;//�������
					//}

					//debug
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					//debug
				}
				break;
			}	
		}
		break;
//-------------------------------------------------------------
		case I2C_TRANSFER_BYTE:	 //�������� ������� ����  //full
		{
			MDE=1;
			//puts("I2C_TRANSFER_BYTE\n");
		   	switch(i2c_st.TRANSFER_BYTE_STATE) //�������� ����� �� ��������� ���������
			{
//--------------
				case 0:
				{	
					//puts("I2C_TRANSFER_BYTE1\n");
										
					if(bit_counter>=8)	//���� �������, ����� ���������, � ���� ����������� ���, ��������� � ����������
					{
					//	byte_transfered_flag=1;//������ ��������
						bit_counter=0;
						//if(rec_tr_flag)
						//{				
						transfer_byte=i2c_buf[transfer_byte_counter];
						transfer_byte_counter++;
						i2c_st.GLOBAL_STATE=I2C_GET_ACK;
						//byte_transfered_flag=0;
						//debug
						i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
						delay_counter=1;
						i2c_st.GLOBAL_STATE=I2C_DELAY;
						//debug
						//}
					}
					else
					{
						transfer_byte=transfer_byte<<1;//>>1;	 //???
						MDO=CY;
						
						i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
						delay_counter=1;
						i2c_st.GLOBAL_STATE=I2C_DELAY;
	
						bit_counter++;
	
						i2c_st.TRANSFER_BYTE_STATE=1;//
	
	
							MCO=1;//clock �����	
					}
				}
				break;
//--------------
				case 1:
				{

			  //debug
				   //debug
				//puts("I2C_TRANSFER_BYTE2\n");

						if(MCO)	  // slave ������ ������������ � �����, ����
						{
							MCO=0;
							i2c_st.TRANSFER_BYTE_STATE=0;
						}
										//debug
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					//debug
				}
				break;
//--------------
			}
		}
		break;
//-------------------------------------------------------------
		case I2C_RECIEVE_BYTE:	 //��������� ������ �����	//full
		{
		   	MDE=0;
			//puts("I2C_RECIEVE_BYTE\n");
			switch(i2c_st.RECIEVE_BYTE_STATE) //�������� ����� �� ��������� ���������
			{
//--------------
				case 0:	 //	 ��������� clock � ����
				{	
					MCO=1;
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.RECIEVE_BYTE_STATE=1;
				}
				break;
//--------------

				case 1:
				{
					MCO=0;//
					recieve_byte=(recieve_byte<<1)|MDI;
					bit_counter++;
					i2c_st.RECIEVE_BYTE_STATE=0;
					if(bit_counter>=8)	//������� ����-������� ACK!
					{
						 i2c_buf[recieve_byte_counter]=recieve_byte;
						 recieve_byte_counter++;
						 bit_counter=0;
						 if(recieve_byte_counter>=byte_num)
						 {
						 	 i2c_st.GLOBAL_STATE=I2C_SET_NACK;	//���� ��� ����� ������� -�������� NACK
						 }
						 else
						 {										// ����� ACK���
						 	 i2c_st.GLOBAL_STATE=I2C_SET_ACK;
						 }
						//debug
						i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
						delay_counter=1;
						i2c_st.GLOBAL_STATE=I2C_DELAY;
						//debug
					}
				}
				break;
//--------------
			}
		}
		break;
//-------------------------------------------------------------
		case I2C_DELAY:	 //��������� �������� //full
		{
			//puts("I2C_DELAY\n");
			delay_counter--;
			if(!delay_counter)
			{
				i2c_st.GLOBAL_STATE=i2c_st.LAST_GLOBAL_STATE ;
			}
		}
		break;
//-------------------------------------------------------------
		case I2C_GET_ACK:  //full  ???
		{
		   	MDE=0;		   // ��������� ���������
			//puts("I2C_GET_ACK\n");
			switch(i2c_st.GET_ACK_STATE) //��������  �� ��������� ���������
			{
//--------------
				case 0:
				{
					MCO=1;
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=1;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					i2c_st.GET_ACK_STATE=1;//
					
				}
				break;
//--------------
				case 1:
				{
					MCO=0;
					if(!MDI) //ACK
					{
						//printf("SLAVE: ACK , %u bit\n",(unsigned int)bit_counter);
						if(!rec_tr_flag)//���� �������� �����
						{
							if(transfer_byte_counter>byte_num)//���� ��� ������ ��������, ���� �� ����
							{
								if(!repeated_strt_read)
								{
									i2c_st.GLOBAL_STATE= I2C_STOP;
									
								}
								else
								{
									i2c_st.GLOBAL_STATE=I2C_REPEAT_START;
								} 
								//data_transfered_flag=1;//��� ������ ��������	
							}
							else//���� �� ���-���� �� ��������
							{
							//printf("symbols %u = %u \n",(unsigned int)transfer_byte_counter,(unsigned int)byte_num) ;
								i2c_st.GLOBAL_STATE= I2C_TRANSFER_BYTE;
							}	
						}
						else//���� ��������� �����
						{
							  i2c_st.GLOBAL_STATE= I2C_RECIEVE_BYTE; //���� �� ����� ������
						}						

						MDE=1;
					}
					else  //NACK
					{
						//printf("SLAVE: NACK , %u bit\n",(unsigned int)bit_counter);
						i2c_st.GLOBAL_STATE=I2C_STOP;
						operation_status=1;//�������� ��������� ���������	
					}
					i2c_st.GET_ACK_STATE=0;//

					//debug
					i2c_st.LAST_GLOBAL_STATE=i2c_st.GLOBAL_STATE;//��������� ���������
					delay_counter=10;
					i2c_st.GLOBAL_STATE=I2C_DELAY;
					//debug
				}
				break;
//--------------					 	
			}
		}
		break;
//-------------------------------------------------------------
		case I2C_WAIT_FOR_IO:  //full
		{
			//�����
		}
		break;
//-------------------------------------------------------------
		case I2C_RESTART: //������� �������
		{
			//��������������� ����������...
			 //transfer_byte=0;
			 //puts("I2C_RESTART\n");
			 recieve_byte=0;
			 recieve_byte_counter=0;
			 transfer_byte_counter=0;
			 bit_counter=0;
			 second_start=0;
			 data_transfered_flag=0;
			 byte_transfered_flag=0;
			 repeated_strt_read=0;
			 i2c_st.LAST_GLOBAL_STATE=I2C_WAIT_FOR_IO;

			 i2c_st.START_STATE=0;
			 i2c_st.STOP_STATE=0;
			 i2c_st.RESTART_STATE=0;
			 i2c_st.SET_ACK_STATE=0;
			 i2c_st.GET_ACK_STATE=0;
			 i2c_st.SET_NACK_STATE=0;
			 i2c_st.TRANSFER_BYTE_STATE=0;
			 i2c_st.RECIEVE_BYTE_STATE=0;
			//--------------------------------------
			operation_status=0;
			i2c_st.GLOBAL_STATE=I2C_WAIT_FOR_IO;//������ �� ��������
		}
		break;
	}

return;
}
//---------------------------------------------------------------------------------------------
void I2C_Is_Write(void)//���� ������ ������ �������
{
   //op_flag++;	
   //puts("Write succesfull\n");
return;
}
//---------------------------------------------------------------------------------------------
void I2C_Is_Read(void)	//���� ������ ������ �������
{
  //puts("Read succesfull\n");
   channels[9].channel_data=(unsigned long*)(&i2c_mas)[0];
return;
}
//---------------------------------------------------------------------------------------------