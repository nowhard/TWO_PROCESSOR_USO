#ifndef PREFERENCES_H
#define	PREFERENCES_H

//#pragma NOAREGS

//#define F_OSC_12  12582912UL//������� ����-12MHz
#define F_OSC_6 6291456


//#define BAUDRATE_9600
#define BAUDRATE_57600
//#define BAUDRATE_38400



//----------------------------------------------------------------------
#ifdef BAUDRATE_9600
	#ifdef F_OSC_12
	//	#define TH1_VAL 	0xEB//0xD7
	//	#define TL1_VAL		0x85//0x0A;
		#define T3CON_VAL	0x86
		#define T3FD_VAL	0x12
	#endif
	
	#ifdef F_OSC_6
		//#define TH1_VAL 	0xF5
		//#define TL1_VAL		0xC3;
		#define T3CON_VAL	0x85
		#define T3FD_VAL	0x12
	#endif
#endif

#ifdef BAUDRATE_57600 //
	#ifdef F_OSC_12
		//#define TH1_VAL 	0xFC
	//	#define TL1_VAL		0x96;
		#define T3CON_VAL	0x83
		#define T3FD_VAL	0x2D
	#endif
	
	#ifdef F_OSC_6
	//	#define TH1_VAL 	0xFE
	//	#define TL1_VAL		0x4B;
		#define T3CON_VAL	0x82
		#define T3FD_VAL	0x2D
	#endif
#endif

#ifdef 	BAUDRATE_38400 //
	#ifdef F_OSC_12
	//	#define TH1_VAL 	0xFA
	//	#define TL1_VAL		0xE3//0xE1
		#define T3CON_VAL	0x84
		#define T3FD_VAL	0x12
	#endif
#endif
//----------------------------------------------------------------------
#ifdef 	F_OSC_12
	#define PLLCON_VAL 0xF8
	
	#define TH0_VAL 	0xFF//0xCE //200Hz
	#define TL0_VAL 	0xFF//0xD9

	#define TH1_VAL 	0x15
	#define TL1_VAL		0xA0//0xE1

	#define F_OSC F_OSC_12
#endif

#ifdef F_OSC_6
	#define PLLCON_VAL 0xF9

	#define TH0_VAL 0xFF //1000Hz
	#define TL0_VAL 0xFF

	#define TH1_VAL 	0x8A
	#define TL1_VAL		0xD0//0xE1

	#define F_OSC F_OSC_6
#endif
//----------------------------------------------------------------------


//----------------------------------------------------------------------

#define DEVICE_ADDR_MAX	0xF //������������ ����� ����������
//----------------------------������ �������� � ����--------------------

#define ADC_SETTINGS_ADDR	0x10


#define DEVICE_ADDR_EEPROM	0x2A

#define DEVICE_NAME_EEPROM		0x2B
#define DEVICE_NAME_LENGTH		0x5//����� ����� � ������
#define DEVICE_NAME_LENGTH_SYM	20//����� ����� � ������

#define DEVICE_VER_EEPROM	0x30
#define DEVICE_VER_LENGTH	0x2//����� ������ �� � ������-����-4�����
#define DEVICE_VER_LENGTH_SYM 0x8

#define DEVICE_DESC_EEPROM	0x33
#define DEVICE_DESC_MAX_LENGTH	10//������������ ����� ������ ��������-5 ������	�� 4 �����
#define DEVICE_DESC_MAX_LENGTH_SYM 40

#define DEVICE_DESC_LEN_EEPROM	0x32//����� �������� ����� ������ �������� � ����

#define ADC_CALIBRATE_ADDR		0x50


#define SETTINGS_DEVICE_CRC_ADDR	0xA0//����� ������������ ����������� ����� �������� ����������
#define CALIBRATE_DEVICE_CRC_ADDR	0xA1//����� ������������ ����������� ����� ���������� ����������


//----------------------------interfaces--------------------------------
#define RS_232
//#define RS_485
//----------------------------------------------------------------------
//----------�������� ����������� ���������-----------------------------
#ifdef 	F_OSC_12
	#define GET_MID_TIM 		1	
	#define FRQ_MEAS_PROC_TIM	50
	#define SEC_TSK_TIM			200
	#define SK_TSK_TIM			200

#endif

#ifdef F_OSC_6
	#define GET_MID_TIM 		1	
	#define FRQ_MEAS_PROC_TIM	50
	#define SEC_TSK_TIM			100
	#define SK_TSK_TIM			30
#endif
//---------------------------------------------------------------------
#endif
