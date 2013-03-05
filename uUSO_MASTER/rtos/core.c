#include "core.h"

// ��������� ������ �� ������ ������������
typedef struct{
  msg_num msg; // �������������� �������
  handler hnd; // ���������� ��� ����������
} iHandler;

// ��������� ������� �� ������ �������
typedef struct{
  msg_num msg; // ����� �������
  msg_par par; // ��������
} iMessage;

// ��������� �������
typedef struct{
  msg_num msg; // ����� ������������� ���������
  msg_par par; // ��� ��������
  unsigned int time; // ������ � �������� ����� (������ 1 ����)
  unsigned int const_time;
} iTimer;



//-----------------����������� ���������� ����������� ������ ��� ����������
/*typedef struct{
  	msg_num msg; // ����� ������������� ���������
  	msg_par par; // ��� ��������� �������� ����� 
	unsigned int time; // ������ � �������� ����� (������ 1 ����)
   	unsigned int const_time;
} iConstTimer; */
//-----------------

 volatile iTimer  		idata  lTimer[maxTimers]={0}; // ������ ��������
// volatile iConstTimer   idata  lConstTimer[maxConstTimer];//������ ���������� ��������

 volatile iHandler  	idata lHandler[maxHandlers]={0}; // ������ ������������

 volatile iMessage 		idata lMessage[maxMessages]={0}; // ����� ���������
 volatile unsigned int  idata lMesPointer=0,hMesPointer=0; // ��������� �� ������ � ����� ������

 volatile unsigned char dispatch_timers_flag=0;//���� -������� ��������� ��������

//------------------------------------------------------------------------------
// ��������� ����������� �������
// ����������: setHandler(MSG_KEY_PRESS, &checkKey);
void setHandler(msg_num msg_pr, handler hnd_pr) //using 2   ///���������
{
  unsigned char   idata i=0,j=0;

    msg_num idata msg;
    handler idata hnd;

  msg=msg_pr;
  hnd=hnd_pr;


  while (i<maxHandlers) {
    if (lHandler[i].msg==0) { // ���� ��������� �����
      lHandler[i].hnd = hnd; // � ����������� ����������
      lHandler[i].msg = msg;
      break;
    }
    i++;
  }
  return;
}
//------------------------------------------------------------------------------
// ������ ����������� �������
// ����������: killHandler(MSG_KEY_PRESS, &checkKey);
/*void killHandler(msg_num msg_pr, handler hnd_pr) //using 2  //���������
{
  unsigned char   xdata  i=0,j=0;
    msg_num idata msg;
    handler idata hnd;

  msg=msg_pr;
  hnd=hnd_pr;


  while (i<maxHandlers) {
    
    if ((lHandler[i].msg==msg) && (lHandler[i].hnd==hnd)) {
      lHandler[i].msg = 0; // ���� ����� ������, �������
    }
    
    if (lHandler[i].msg != 0) {
      if (i != j) { // �������� ��� ������ � ������ ������, ����� ����� �� ����
        lHandler[j].msg = lHandler[i].msg;
        lHandler[j].hnd = lHandler[i].hnd;
        lHandler[i].msg = 0;
      }
      j++;
    }
    i++;
  }
  return;
}*/
//------------------------------------------------------------------------------
// ������� ������� � �������
// ������ ������: sendMessage(MSG_KEY_PRESS, KEY_MENU)
void sendMessage(msg_num msg_pr, msg_par par_pr) //using 2	//���������
{
  
    msg_num msg;
    msg_par par;

  msg=msg_pr;
  par=par_pr;

  hMesPointer = (hMesPointer+1) & (maxMessages-1); // �������� ��������� ������
  
  lMessage[hMesPointer].msg = msg; // ������� ������� � ��������
  lMessage[hMesPointer].par = par;
  if (hMesPointer == lMesPointer) 
  { // ������� ������ �������, ������� �������������� ��������
    lMesPointer = (lMesPointer+1) & (maxMessages-1);
  }
  return;
}
//------------------------------------------------------------------------------
// ��������� �������
void dispatchMessage(void) //using 2
 {	//�������� ��������, ��������������
     char xdata  i=0;
  unsigned char   xdata res; //���������
  msg_num   xdata msg;
  msg_par   xdata par;
  
	if(dispatch_timers_flag==1)
	{
		 dispatchTimer(); 
		 dispatch_timers_flag=0;
	}

  if (hMesPointer == lMesPointer) 
  { // ���� ������ ������� - �������
    return;	
  }
  
  lMesPointer = (lMesPointer+1) & (maxMessages-1); // ������� ���������
  
  msg = lMessage[lMesPointer].msg;
  par = lMessage[lMesPointer].par;
  
  if (msg==0)
    return;

  
  for(i=maxHandlers-1; i>=0; i--) 
  { 
  // ������������� ����������� � �����
    if (lHandler[i].msg==msg) 
	{ 
	// ��������� ���������� ����� ���������
      res = lHandler[i].hnd(par); // �������� ����������
		 
       if (res)
	   { // ���� ���������� ������ 1, �������� ��������� �������
        break;
       }
    }
  }
  return;
}
//------------------------------------------------------------------------------
// ���������� ������
// ������ ������: setTimer(MSG_LCD_REFRESH, 0, 50);
//#pragma ot(6)
void setTimer(msg_num msg_pr, msg_par par_pr, unsigned int time_pr) //using 2   //���������
{
  	unsigned char i=0,firstFree=0;

	msg_num idata msg;
	msg_par idata par;
	unsigned  int idata time;
  
  msg=msg_pr;
  par=par_pr;
  time=time_pr;	



  firstFree = maxTimers+1;
 /* if (time == 0) 
  {
    sendMessage(msg, par);
  } 
  else */
  {
  
    for (i=0; i<maxTimers; i++) 
	{ 
	   // ���� ������������� ������
      if (lTimer[i].msg == 0) 
	  {
        firstFree = i;
      } 
	  else 
	  { 
	  // ���� ����� - ��������� �����
        if ((lTimer[i].msg == msg) && (lTimer[i].par == par))
	    {
          lTimer[i].time = time;
		  lTimer[i].const_time = time;
          return;
        }  
      }
    }
    if (firstFree <= maxTimers)
	{ // ����� - ������ ��������� �����
      lTimer[firstFree].msg = msg;
      lTimer[firstFree].par = par;
      lTimer[firstFree].time = time;
	  lTimer[firstFree].const_time = time;
    }
  }
  return;
}
//------------------------------------------------------------------------------
// ����� ������
// ����������� - ������� ��� ������������� ������� �� ������ �������,
// �� �������� �� ��������� �������
/*void killTimer(msg_num msg_pr) //using 2	//���������
{
    unsigned char xdata i=0;
    msg_num idata msg;
  msg=msg_pr;

  for (i=0; i<maxTimers; i++) {
    if (lTimer[i].msg == msg) {
      lTimer[i].msg = 0;
    }
  }
  return;
} */
//------------------------------------------------------------------------------
// ��������� ��������
void dispatchTimer() 
{	 //������ ��������
 	unsigned char   xdata i=0;
    msg_num idata msg;
    msg_par idata par;

//------------��������� ��������� ��������----------------------------------- 
  for (i=0; i<maxTimers; i++) 
  {    
		if (lTimer[i].msg == 0)
		{
	      continue;
		}
	    
	    if (lTimer[i].time == 0) 
		{ 
		// ���� ������ �����
	      msg = lTimer[i].msg;
	      par =lTimer[i].par;
	      //lTimer[i].msg = 0;
		  lTimer[i].time=lTimer[i].const_time;
	 
	      //sendMessage(msg, par); // ������� �������
		  //-----------------------------------------------
	      hMesPointer = (hMesPointer+1) & (maxMessages-1); // �������� ��������� ������

		  lMessage[hMesPointer].msg = msg; // ������� ������� � ��������
		  lMessage[hMesPointer].par = par;
		  if (hMesPointer == lMesPointer) 
		  { // ������� ������ �������, ������� �������������� ��������
		    lMesPointer = (lMesPointer+1) & (maxMessages-1);
		  }

		  //-----------------------------------------------
	    } 
	else 
	{
      lTimer[i].time--; // ����� ������ ��������� �����
    }
  }
   //---------------------------------------

  return;
} 
//------------------------------------------------------------------------------
void Timer1_Interrupt(void) interrupt 3  //using 1
{
//---------------------------------------
    dispatch_timers_flag=1;

	TH1	= TH1_VAL; ///200 Hz;
	TL1 = TL1_VAL;//

	return;	
}

//------------------------------------------------

