#ifndef THERMOCOMP_H
#define	THERMOCOMP_H
#include "adc.h"
#include "eeprom/eeprom.h"

//-----------------------------------------------------------------------------
/*#define TABLE_LEN 	100
#define MIN_TEMP	0
#define MAX_TEMP	100
#define STEP_TEMP	1

#define TABLE_EEPROM_ADDR	0x20 */

#define FRST_PNT_THM_ADDR	0x10
#define FRST_PNT_VLT_ADDR	0x11

#define SCND_PNT_THM_ADDR	0x12
#define	SCND_PNT_VLT_ADDR	0x13
//-----------------------------------------------------------------------------
static float first_thm, second_thm;
static float first_vlt,second_vlt;
static float thm_coeff;
//-----------------------------------------------------------------------------
//void FillTermTable(void);//���������� ������� ����������������
//void StoreTermTable(void);//���������� ������� ����������������	� EEPROM
//void RestoreTermTable(void);//�������� ������� ���������������� �� EEPROM
//long TermCompensate(long temp,long pressure);//������� ��������� ����������������

void SetFirstThmPoint(float temp, float	voltage); //��������� ������ ����� ����������������
void SetSecondThmPoint(float temp, float voltage); //��������� ������ ����� ����������������

void SetCompensate(unsigned char point);//��������� ����� �����������
void RestoreCompensate(void);//�������������� �������������������� ������������ ����� ���������

float Compensate(float voltage,float temp);//������� ����������� 



//-----------------------------------------------------------------------------
//static long  volatile TermTable[100];//������� ���������������� � ���
/*��������� �������*/
//TermTable[0] ->coeff 20 C
//TermTable[1] ->coeff 21 C
//TermTable[2] ->coeff 22 C
//..........................
//..........................
//------------------------------------------------------------------------------
//static char table_counter=0;
#endif