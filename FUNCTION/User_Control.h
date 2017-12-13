#ifndef __FUNTION_H
#define __FUNTION_H


#include "stm32f10x.h"
#include "stm32f10x_it.h"


/*PID �㷨���ӿڲ�ʽṹ�����*/
typedef struct 
{

 float kp;     //����ϵ��
 float ki;     //����ϵ��
 float kd;     //΢��ϵ��
 float errILim;//����������
 
 float errNow;//��ǰ�����
 float ctrOut;//���������
 
 
 float errOld;
 float errP;
 float errI;
 float errD;
 
}PID_AbsoluteType;

void get_encoder_Left(void);
extern void User_PidSpeedControl(s32 SpeedTag);


#endif





