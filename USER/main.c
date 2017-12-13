#include "string.h"
#include "BSP_init.h"

#include "contact.h"
#include "odometry.h"

#include "usb_lib.h"
#include "usbio.h"
#include "us_can_zyt.h"
#include "us_mcu_transfer.h"

UART_INFO send_buf;

/***********************************************  ���  *****************************************************************/

char odometry_data[21]={0};   //���͸����ڵ���̼���������

float odometry_right=0,odometry_left=0;//���ڵõ����������ٶ�

/***********************************************  ����  *****************************************************************/

extern float position_x,position_y,oriention,velocity_linear,velocity_angular;         //����õ�����̼���ֵ

extern u8 USART_RX_BUF[USART_REC_LEN];     //���ڽ��ջ���,���USART_REC_LEN���ֽ�.
extern u16 USART_RX_STA;                   //���ڽ���״̬���	

extern float Milemeter_L_Motor,Milemeter_R_Motor;     //dtʱ���ڵ��������ٶ�,������̼Ƽ���

/***********************************************  ����  *****************************************************************/

u8 main_sta=0; 				


union recieveData  		//���յ�������
{
	float d;    		 		//�������ٶ�
	unsigned char data[4];
}leftdata,rightdata;  //���յ�����������


union odometry  	 		//��̼����ݹ�����
{
	float odoemtry_float;
	unsigned char odometry_char[4];
}x_data,y_data,theta_data,vel_linear,vel_angular;     
//Ҫ��������̼����ݣ��ֱ�Ϊ��X��Y�����ƶ��ľ��룬��ǰ�Ƕȣ����ٶȣ����ٶ�

/****************************************************************************************************************/	

 int main(void)
 {	 

#if 0
	 u8 t=0;
	 u8 i=0,j=0,m=0;
#endif

		UART_INFO *send_ptr = &send_buf;
		uint8_t *send = (uint8_t *)send_ptr;
		int send_size = 0;
	 
	 BSP_Configuration();		//�弶��Դ ��ʼ��
	 us_mcu_id_get();				//US Get MCU ID
	 
	 while(1)
	 {  
			delay_ms(100);
			odometry(Milemeter_R_Motor,Milemeter_L_Motor);//������̼�		 
		 
			if(GetEPTxStatus(ENDP2) == EP_TX_NAK){
				if(us_mcu_rc_buff_delete(send_ptr) == OK){
					if((send_size = us_mcu_uart_coder(send_ptr)) < 0){
							us_dev_error(MCU_CONFIG, (unsigned char *)__func__, strlen(__func__)+1, send_size);
					}else{
							USB_SendData(send, send_size);
					}
				}
			}

			
#if 0
			//ִ�з�����̼����ݲ���
			if(main_sta&0x01)
			{
					//��̼����ݻ�ȡ
					x_data.odoemtry_float=position_x;			//��λmm
					y_data.odoemtry_float=position_y;			//��λmm
					theta_data.odoemtry_float=oriention;		//��λrad
					vel_linear.odoemtry_float=velocity_linear;	//��λmm/s
					vel_angular.odoemtry_float=velocity_angular;//��λrad/s
							
					//��������̼����ݴ浽Ҫ���͵�����
					for(j=0;j<4;j++)
					{
						odometry_data[j]=x_data.odometry_char[j];
						odometry_data[j+4]=y_data.odometry_char[j];
						odometry_data[j+8]=theta_data.odometry_char[j];
						odometry_data[j+12]=vel_linear.odometry_char[j];
						odometry_data[j+16]=vel_angular.odometry_char[j];
					}
							
					odometry_data[20]='\n';//��ӽ�����
							
					//��������Ҫ����
					for(i=0;i<21;i++)
					{
						USART_ClearFlag(USART1,USART_FLAG_TC);  //�ڷ��͵�һ������ǰ�Ӵ˾䣬�����һ�����ݲ����������͵�����				
						USART_SendData(USART1,odometry_data[i]);//����һ���ֽڵ�����	
						while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//�ȴ����ͽ���			
					}
							
					main_sta&=0xFE;//ִ�м�����̼����ݲ���
			}
		

	
			//ִ�м�����̼����ݲ���
			if(main_sta&0x02)
			{
				odometry(Milemeter_R_Motor,Milemeter_L_Motor);//������̼�
				main_sta&=0xFD;//ִ�з�����̼����ݲ���
			} 
		
			
	
			//������ָ��û����ȷ����ʱ
			if(main_sta&0x08)        
			{
					USART_ClearFlag(USART1,USART_FLAG_TC);  //�ڷ��͵�һ������ǰ�Ӵ˾䣬�����һ�����ݲ����������͵�����
					for(m=0;m<3;m++)
					{
							USART_SendData(USART1,0x00);	
							while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
					}		
					USART_SendData(USART1,'\n');	
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	
					main_sta&=0xF7;
			}
			
			
		
			// ����1���պ���
			if(USART_RX_STA&0x8000)  
			{			
				//�����������ٶ�
				for(t=0;t<4;t++)
				{
						rightdata.data[t]=USART_RX_BUF[t];
						leftdata.data[t]=USART_RX_BUF[t+4];
				}

				//�����������ٶ�
				odometry_right=rightdata.d;
				odometry_left=leftdata.d;

				USART_RX_STA=0;//������ձ�־λ
			}
		
			
			//�����յ����������ٶȸ���С��
			car_control(rightdata.d,leftdata.d);
#endif			
	 }	 //end while
 }	//end main

