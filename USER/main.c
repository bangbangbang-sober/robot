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
		UART_INFO *send_ptr = &send_buf;
		uint8_t *send = (uint8_t *)send_ptr;
		int send_size = 0;
	 
	 BSP_Configuration();		//�弶��Դ ��ʼ��
	 us_mcu_id_get();				//US Get MCU ID

	 while(1)
	 {  			
		 		if(GetEPTxStatus(ENDP2) == EP_TX_NAK){
					 if(us_mcu_rc_buff_delete(send_ptr) == OK){
							if((send_size = us_mcu_uart_coder(send_ptr)) < 0){
									us_dev_error(MCU_CONFIG, (unsigned char *)__func__, strlen(__func__)+1, send_size);
						 }else{
								 USB_SendData(send, send_size);
						 }
					 }
				 }				 
	 }	 //end while
 }	//end main

