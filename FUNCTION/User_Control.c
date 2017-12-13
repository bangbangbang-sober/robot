#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "User_Control.h"

//���ݱ����������� 600��������600*4��,  ����ֱ��  38mm   �õ�һ�������Ӧ����
#define Get_OnePulse_length  (3.1415926 * 38 / (600 * 4))

#define PWMPeriod   2400
#define prd    			2400
#define Vbreak 			2200

u16 Cnt_Left;
s32 CNT_L;
s32 V_Left;

extern s32 Rcnt_L;

void get_encoder_Left(void)//*******************���㵱ǰʵ���ٶ�
{
  s32 CNTL_temp,CNTL_last;
  
  Cnt_Left = TIM3 -> CNT;
  CNTL_last = CNT_L;
  CNTL_temp = Rcnt_L * prd + Cnt_Left;  
  V_Left = CNTL_temp - CNTL_last;		
  
  while (V_Left>Vbreak)				 
  {							      
    Rcnt_L--;					      
    CNTL_temp = Rcnt_L * prd + Cnt_Left;
    V_Left = CNTL_temp - CNTL_last;		 
  }							     
  while (V_Left<-Vbreak)			   
  {							      
    Rcnt_L++;					      
    CNTL_temp = Rcnt_L * prd + Cnt_Left;
    V_Left = CNTL_temp - CNTL_last;		 
  }
	
  CNT_L = CNTL_temp;						 
  
}

void UserMotorSpeedSetOne(s32 control)//���ת�ٿ���
{		
	s32 MotorSpeed;

	MotorSpeed = control;//��ȡPID������ֵ
											   
	if(MotorSpeed > PWMPeriod)  
		MotorSpeed =   PWMPeriod-1 ;
	
	TIM2->CCR2 = MotorSpeed; 
}


//����ʽPID�㷨
void PID_AbsoluteMode(PID_AbsoluteType* PID)
{
    if(PID->kp < 0)    
        PID->kp = -PID->kp;
    if(PID->ki < 0)    
        PID->ki = -PID->ki;
    if(PID->kd < 0)    
        PID->kd = -PID->kd;
    if(PID->errILim < 0)    
        PID->errILim = -PID->errILim;

    PID->errP = PID->errNow;  //��ȡ���ڵ�������KP����

    PID->errI += PID->errNow; //�����֣�����KI����

    if(PID->errILim != 0)	   //΢�����޺�����
    {
        if(PID->errI >  PID->errILim)    
            PID->errI =  PID->errILim;
        else if(PID->errI < -PID->errILim)    
            PID->errI = -PID->errILim;
    }

    PID->errD = PID->errNow - PID->errOld;//���΢�֣�����kd����

    PID->errOld = PID->errNow;	//�������е����

    PID->ctrOut = PID->kp * PID->errP + PID->ki * PID->errI + PID->kd * PID->errD;//�������ʽPID���

}

s32 spdTag, spdNow, control;	//����һ��Ŀ���ٶȣ������ٶȣ�������
PID_AbsoluteType PID_Control;	//PID�㷨�ṹ��

//pid����ת��
void User_PidSpeedControl(s32 SpeedTag)
{
   spdNow = V_Left; spdTag = SpeedTag;

   PID_Control.errNow = spdTag - spdNow; //���㲢д���ٶ����
   	
   PID_Control.kp      = 15;             //����ϵ����15
   PID_Control.ki      = 5;              //����ϵ����5
   PID_Control.kd      = 5;              //΢��ϵ����5
   PID_Control.errILim = 1000;           //���������ޣ�1000 ���ޣ�-1000

   PID_AbsoluteMode(&PID_Control);       //ִ�о���ʽPID�㷨
	
   control = PID_Control.ctrOut;         //��ȡ����ֵ

   UserMotorSpeedSetOne(control);        //����PWM�����������ٶȿ�����

}

//�õ����г��ȣ���������Ҫ������ٸ�������
int Get_Pulse(u32 length)
{
	return (length / Get_OnePulse_length);
}










