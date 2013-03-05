#ifndef EEPROM_H
#define EEPROM_H
#include <ADuC845.h>

//---------------------------------------
//������,������ � �������� ����������� 
//����������������� ����
//---------------------------------------
#define EE_READ_PAGE	0x1	 //������� ������
#define EE_WRITE_PAGE	0x2	 //������� ������
#define EE_VERIFY_PAGE	0x4	 //�������� ������ ��������
#define EE_ERASE_PAGE	0x5	 //�������� ��������
#define EE_ERASE_ALL	0x6  //�������� ���
#define EE_READ_BYTE	0x81 //��������� ����
#define EE_WRITE_BYTE 	0x82 // �������� ���� 

//---------------------------------------
#define EEMEM_SIZE 		1024 //������ ������ � ���������
//---------------------------------------
sfr16 EADR=0xC6;
//---------------------------------------
unsigned int EEPROM_Read(void *buffer,unsigned int len,unsigned int addr);//��������� ��������� ������� � ������
unsigned int  EEPROM_Write(void *buffer,unsigned int len,unsigned int addr);//�������� ����� �� ������
void 		  EEPROM_Erase(void);// 
#endif 