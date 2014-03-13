#ifndef PROTO_USO_H
#define PROTO_USO_H

#include <ADuC845.h>

#include "adc.h"
#include "calibrate/calibrate.h"
#include "ulongsort.h"
#include <string.h>


#include "pt/pt.h"

//--------------------------------------------------------------------------------
#define MAX_LENGTH_REC_BUF 	270 //������������ ����� ������������ �����
#define MIN_LENGTH_REC_BUF	5 //����������� ����� ������������ �����

#define MAX_LENGTH_TR_BUF  	270 //������������ ����� ������������� �����
#define CRC_LEN				1 //����� ���� CRC
//-------------------------��� ��������-------------------------------------------
#define  GET_DEV_INFO_REQ 				0x1 //�������� ���������� �� ����������	(��� �������)
#define  GET_DEV_INFO_RESP				0x2	//�������� ���������� �� ����������	(��� ������)

#define  NODE_FULL_INIT_REQ				0x3	//������ ������������� ����
#define  NODE_FULL_INIT_RESP			//������ ������������� ����-��� ������

#define  CHANNEL_LIST_INIT_REQ			0x5 //������������� ������ ������� ���� (��� ������ ������);
#define  CHANNEL_LIST_INIT_RESP			//������������� ������ ������� ���� (��� ������ ������); ��� ������

#define  CHANNEL_GET_DATA_REQ			0x7// ������ ������ �� �������, �������� ���������� ���������;
#define  CHANNEL_GET_DATA_RESP			0x8// ������ ������ �� �������, �������� ���������� ���������;

#define  CHANNEL_SET_PARAMETERS_REQ  	0x9	//���������� ��������� �� �������, �������� ���������� ���������;	
#define  CHANNEL_SET_PARAMETERS_RESP  	//���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define  CHANNEL_SET_ORDER_QUERY_REQ   	0xA//������ ������������������ ������;
#define  CHANNEL_SET_ORDER_QUERY_RESP 	//������ ������������������ ������; ��� ������

#define  CHANNEL_GET_DATA_ORDER_REQ		0xC	//������ ������ �� �������, �������� ������������������ ������;
#define  CHANNEL_GET_DATA_ORDER_RESP	0xD	//������ ������ �� �������, �������� ������������������ ������;

#define  CHANNEL_SET_STATE_REQ		   	0xE//���������� ��������� �� �������, �������� ���������� ���������;
#define  CHANNEL_SET_STATE_RESP			   //���������� ��������� �� �������, �������� ���������� ���������; ��� ������

#define CHANNEL_GET_DATA_ORDER_M2_REQ	0x10 //������ ������ �� �������, �������� ������������������ ������;
#define CHANNEL_GET_DATA_ORDER_M2_RESP	0x11//������ ������ �� �������, �������� ������������������ ������;

#define CHANNEL_SET_RESET_STATE_FLAGS_REQ  0x12//���������/����� ������ ��������� 
#define CHANNEL_SET_RESET_STATE_FLAGS_RESP //���������/����� ������ ��������� -��� ������

#define  CHANNEL_ALL_GET_DATA_REQ		0x14 //������ ���������� �� ���� ������� ���� (����������� �����);
#define  CHANNEL_ALL_GET_DATA_RESP		0x15 //������ ���������� �� ���� ������� ���� (����������� �����);

#define  CHANNEL_SET_ADDRESS_DESC		0xCD //���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������

#define  CHANNEL_SET_CALIBRATE			0xCA//���������� ������� ��� ������ ����� ������������ ����������

#define  CHANNEL_GET_CALIBRATE_REQ		0xCB//�������� ����������� ���������� ��������� ������
#define  CHANNEL_GET_CALIBRATE_RESP		0xCC//�������� ����������� ���������� ��������� ������

#define  CHANNEL_SET_ALL_DEFAULT		0xDF //���������� ��� �������� �� ���������

#define  REQUEST_ERROR					0xFF//��������� ������/�����;



#define PROTO_ADC_AMP_1	 		0
#define PROTO_ADC_AMP_2			1
#define PROTO_ADC_AMP_32		5
#define PROTO_ADC_AMP_128		7
//-------------------------���� ������� ��������-------------------------------------------
#define	FR_SUCCESFUL 								0x0//��� ������ (������������ ��� �������������)  
#define	FR_UNATTENDED_CHANNEL 						0x1//� ������� ����� �����, �� ������������� ������������� �������;
#define	FR_SET_UNATTENDED_CHANNEL					0x2//� �������� ������������������ ������ ����� �����, �� ������������� ��-����������� ������� (�������� � ����� �� �������� ������� 0Ah);
#define	FR_ORDER_NOT_SET							0x3//������������������ ������ �� ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_CHNL_NOT_IN_ORDER_REQ					0x4//� ������� ������������ �����, �� �������� � ������������������ ������ (�������� � ����� �� �������� ������� 0Ch);
#define	FR_FALSE_TYPE_CHNL_DATA						0x5//���������� ���������� �� ������ � ������� ���� ������ ���������� �� ����� (�������� � ����� �� �������� ������� 09h ��� 0Eh);
#define	FR_COMMAND_NOT_EXIST						0x6//�������������� �������;
#define	FR_CHNL_TYPE_NOT_EXIST						0x7//�������������� ���/����������� ������;
#define	FR_COMMAND_NOT_SUPPORT						0x8//������� �� ��������������;
#define	FR_COMMAND_STRUCT_ERROR						0x9//������ � ��������� �������;
#define	FR_FRAME_LENGTH_ERROR						0xA//�������������� ����� ����� � ���������� �������;
#define	FR_CHANNEL_NUM_TOO_MUCH						0xC//������� ����� ������� � �����.
#define	FR_CHNL_TYPE_ERROR							0xD//�������������� ���� ������
#define	FR_CHNL_NOT_EXIST							0xF//������������� �����

enum
{
	PROTO_TYPE_NEW=0,
	PROTO_TYPE_OLD=1
};
//--------------------------------���������---------------------------
void UART_ISR(void); //���������� ���������� ����
void Protocol_Init(void); //������������� ���������

unsigned char Send_Info(void);     //������� ���������� �� ����������
unsigned char Node_Full_Init(void);//������ ������������� ����
unsigned char Channel_List_Init(void);//������������� ������ ������� ���� (��� ������ ������);
unsigned char Channel_Get_Data(void);//������ ������ �� �������, �������� ���������� ���������;
unsigned char Channel_Set_Parameters(void);//���������� ��������� �� �������, �������� ���������� ���������;
unsigned char Channel_Set_Order_Query(void);//������ ������������������ ������;
unsigned char Channel_Get_Data_Order(void);//������ ������ �� �������, �������� ������������������ ������;
unsigned char Channel_Set_State(void);//���������� ��������� �� �������, �������� ���������� ���������;
unsigned char Channel_Get_Data_Order_M2(void);//������ ������ �� �������, �������� ������������������ ������;
unsigned char Channel_Set_Reset_State_Flags(void);//	���������/����� ������ ��������� 
unsigned char Channel_All_Get_Data(void);//������ ���������� �� ���� ������� ���� (����������� �����);
unsigned char Channel_Set_Address_Desc(void);//���������� ����� ����� ����������, ���, ��������, ������ �������� � �����������
unsigned char Channel_Set_Calibrate(void);//���������� ������� ��� ������ ����� ����������
unsigned char Channel_Set_All_Default(void);//���������� ��������� � ���������� ������� �� ���������
unsigned char Channel_Get_Calibrate_Value(void);//�������� ����������� ���������� ��������� ������
unsigned char Request_Error(unsigned char error_code);//	��������� ������/�����;
//------------------------------------��������� ������� ��������� IMS----------------------------
#define	OLD_CHANNEL_REINIT_BLOCK 		0x0	 //��������������� �����
#define	OLD_CHANNEL_SET_ADC_RANGE		0x1	 //���������� �������� ���
#define	OLD_CHANNEL_GET_STATE		    0x2	 //
#define	OLD_CHANNEL_GET_DATA			0x3
#define	OLD_CHANNEL_GET_DATA_STATE		0x4
//------------------------------------�������� �������� �������----------------------------
#define OLD_PROTO_ADC_AMP_1	 		0
#define OLD_PROTO_ADC_AMP_2			1
#define OLD_PROTO_ADC_AMP_32		2
#define OLD_PROTO_ADC_AMP_128		3
//-----------------------------------------------------------------------------------------
unsigned char Old_CRC_Check(unsigned char xdata *Spool_pr,unsigned char Count_pr);
unsigned char Old_Channel_Get_Data(void); //�������� ������ ������
unsigned char Old_Channel_Get_Data_State(void);//�������� ������ � ����� ��������� ������
unsigned char Old_Channel_Get_State(void);//�������� ����� ��������� ������
unsigned char Old_Reinit_Block(void);//���������������...
unsigned char Old_Channel_Set_ADC_Range(void);//������� �������� ���������������� ��������� ���
unsigned char Old_Proto_Paste_Null(unsigned char *buf,unsigned char len);//�.�. ������ �������� ���������� � ����� ���������� 0 ����� D7 �� ������������ ��� ����������
//-----------------------------------------------------------------------------------------------
void ProtoBufHandling(void); //������� ��������� ��������� �������
PT_THREAD(ProtoProcess(struct pt *pt));//������� ������� ���������

static unsigned char  CRC_Check( unsigned char xdata *Spool,unsigned char Count);//������ CRC

void Store_Dev_Address_Desc(unsigned char addr,void* name,void* ver,void* desc,unsigned char desc_len);//��������� � ���� ����� ����� ����������, ���, ������, ��������
void Restore_Dev_Address_Desc(void);//������������ �� ���� ����� � ���������� �� ����������
 //------------------------------------------------------------------------------
 extern struct Channel xdata channels[CHANNEL_NUMBER];//���������� ��������� �������
 extern struct ADC_Channels xdata adc_channels[ADC_CHANNELS_NUM]; //������ ���
//--------------------------------------------------------------------
#endif