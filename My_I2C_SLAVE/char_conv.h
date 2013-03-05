#ifndef CHAR_CONV_H
#define CHAR_CONV_H

unsigned int Sym_4_To_Int(unsigned char * pucFrame);//������� 4� �������� � ������� � �����	int
unsigned char Sym_2_To_Char(unsigned char * pucFrame);//������� 2� �������� � ������� � ����� char
unsigned long Sym_8_To_Long(unsigned char * pucFrame);//������� 8�� �������� � ������� � ����e long

void Int_To_Sym_4(unsigned int val, unsigned char* buf[]);//������� ������ �������� int � ������ �� 4� ��������
void Char_To_Sym_2(unsigned char val, unsigned char* buf[]);//������� ������ �������� char � ������ �� 2� ��������
void Long_To_Sym_8(unsigned long val, unsigned char* buf[]);//������� ������ �������� long � ������ �� 8�� ��������

#endif