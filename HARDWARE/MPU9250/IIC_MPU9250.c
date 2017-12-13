#include "IIC_MPU9250.h"
#include "myiic.h"
#include "delay.h"
#include "usart.h"
#include "time.h"
#include "usb_lib.h"
#include "us_mcu_transfer.h"

#include "iNEMO_AHRS.h"

//**************************************
//��I2C�豸д��һ���ֽ�����
//**************************************
SJSMPU mpu9250;
short int Gyro_Xout,Gyro_Yout,Gyro_Zout;
float ax,ay,az,gx,gy,gz,mx,my,mz,mx1,my1,mz1;
uint8_t packet_buf[64] = {0};

/**********iNEMO*************/
iNEMO_SENSORDATA xSensorData;
iNEMO_EULER_ANGLES xEulerAngles={0};
iNEMO_QUAT  xQuat={0};


int SJS_iNEMO_Init(void)
{
	int ret = 0;
	
	xSensorData.m_fAccRef[0]=0;
	 xSensorData.m_fAccRef[1]=0;
	 xSensorData.m_fAccRef[2]=-9.81f;
		 
	 xSensorData.m_fMagRef[0]=0.37f;
	 xSensorData.m_fMagRef[1]=0;
	 xSensorData.m_fMagRef[2]=-0.25f;
	 
	 xSensorData.m_fDeltaTime=0.02f;
	 
	 xSensorData.m_fVarAcc=5.346e-6;
	 xSensorData.m_fVarMag=5.346e-6;
	
	
	 iNEMO_AHRS_Init(&xSensorData, &xEulerAngles, &xQuat);

	

	return ret;
}

int SJS_iNEMO_Update(SJSMPU mpu9250)
{
	int ret = 0;
	
#if 0	

	xSensorData.m_fAcc[0]=mpu9250.acc_x*9.8f/1000.0f;
	xSensorData.m_fMag[0]=mpu9250.mag_x/0.6/1000.0;
	xSensorData.m_fGyro[0]=mpu9250.gyro_x/57.3*3.141592f/180.0f;

	xSensorData.m_fAcc[1]=-mpu9250.acc_y*9.8f/1000.0f;
	xSensorData.m_fMag[1]=-mpu9250.mag_z/0.6/1000.0;
	xSensorData.m_fGyro[1]=-mpu9250.gyro_y/57.3*3.141592f/180.0f;

	xSensorData.m_fAcc[2]=-mpu9250.acc_z*9.8f/1000.0f;
	xSensorData.m_fMag[2]=-mpu9250.mag_y/0.6/1000.0;
	xSensorData.m_fGyro[2]=-mpu9250.gyro_z/57.3*3.141592f/180.0f;
#endif

#if 1
	xSensorData.m_fAcc[0]=mpu9250.acc_x*ACC_KEN*9.8f/1000.0f;
	xSensorData.m_fMag[0]=mpu9250.mag_x*0.6/1000.0f;
	xSensorData.m_fGyro[0]=mpu9250.gyro_x/16.4*3.141592f/180.0f;

	xSensorData.m_fAcc[1]=-mpu9250.acc_y*ACC_KEN*9.8f/1000.0f;
	xSensorData.m_fMag[1]=-mpu9250.mag_y*0.6/1000.0f;
	xSensorData.m_fGyro[1]=-mpu9250.gyro_y/16.4*3.141592f/180.0f;

	xSensorData.m_fAcc[2]=-mpu9250.acc_z*ACC_KEN*9.8f/1000.0f;
	xSensorData.m_fMag[2]=-mpu9250.mag_z*0.6/1000.0f;
	xSensorData.m_fGyro[2]=-mpu9250.gyro_z/16.4*3.141592f/180.0f;
#endif

	iNEMO_AHRS_Update(&xSensorData, &xEulerAngles, &xQuat);

	
	us_mcu_rc_buff_enter(SNS_iNEMO_SENSOR_WRITE, (unsigned char *)&xEulerAngles, sizeof(iNEMO_EULER_ANGLES));

	return ret;
}

/****************************/

void IIC_Write_OneByte(u8 SlaveAddress,u8 REG_Address,u8 REG_data)
{
	IIC_Start();                   //��ʼ�ź�
    IIC_Send_Byte(SlaveAddress);   //�����豸��ַ+д�ź�
	IIC_Wait_Ack();	   
    IIC_Send_Byte(REG_Address);    //�ڲ��Ĵ�����ַ
	IIC_Wait_Ack(); 	 										  		   
    IIC_Send_Byte(REG_data);       //�ڲ��Ĵ�������
	IIC_Wait_Ack(); 	 										  		   
    IIC_Stop();                    //����ֹͣ�ź�
}
//**************************************
//��I2C�豸��ȡһ���ֽ�����
//**************************************
u8 IIC_Read_OneByte(u8 SlaveAddress,u8 REG_Address)
{
	u8 REG_data;
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(SlaveAddress);  //�����豸��ַ+д�ź�
	REG_data=IIC_Wait_Ack();	   
	IIC_Send_Byte(REG_Address);   //���ʹ洢��Ԫ��ַ����0��ʼ
	REG_data=IIC_Wait_Ack();	   
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(SlaveAddress+1);//�����豸��ַ+���ź�
	REG_data=IIC_Wait_Ack();	   
	REG_data=IIC_Read_Byte(0);		//��ȡһ���ֽ�,�������ٶ�,����NAK,�����Ĵ�������
	IIC_Stop();	                  //ֹͣ�ź�
	return REG_data;
}

void IIC_MPU9250_init(void)
{
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_PWR_MGMT_1,0x00);						//����mpu9250
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_CONFIG,0x06);    						//��ͨ�˲�5hz
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_GYRO_CONFIG,0x18);					//���Լ죬2000deg/s
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_ACCEL_CONFIG,0x00);					//(0x00 +-2g;)  ( 0x08 +-4g;)  (0x10 +-8g;)  (0x18 +-16g)
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_INT_PIN_CFG,0x02);
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_USER_CTRL,0x00);						//ʹ��I2C

#if 0
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_PWR_MGMT_2,0x07);
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_ACCEL_CONFIG_2,0x05);
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_INT_ENABLE,0x40);						//ʹ��INTR
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_MOT_DETECT_CTRL,0xC0);				//ʹ��INTR
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_MOT_THR, 20);							//ʹ��INTR
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_LP_ACCEL_ODR, 60);						//ʹ��INTR
	IIC_Write_OneByte(MPU9250_I2C_ADDR,MPU9250_PWR_MGMT_1, 20);						//ʹ��INTR
#endif

	IIC_Write_OneByte(AK8963_I2C_ADDR,AK8963_CNTL1,0x10);
	delay_ms(50);
	IIC_Write_OneByte(AK8963_I2C_ADDR,AK8963_CNTL1,0x11);								//������ģʽ+16λ����
}



int get_gyro_bias(void)
{
  	unsigned short int i,j;
  	signed short int gyro[3];
  	float gyro_x=0, gyro_y=0, gyro_z=0;
  	static unsigned short count=0;
		s16 buf[20];
  	for(i=0;i<200;i++)
  	{
     	
		//��ȡ���ٶȡ������Ǵ�����
		IIC_Start();                  //��ʼ�ź�
		IIC_Send_Byte(MPU9250_I2C_ADDR);  //�����豸��ַ+д�ź�
		IIC_Wait_Ack();	   
		IIC_Send_Byte(MPU9250_ACCEL_XOUT_H);   //���ʹ洢��Ԫ��ַ����0��ʼ
		IIC_Wait_Ack();	   
		IIC_Start();                  //��ʼ�ź�
		IIC_Send_Byte(MPU9250_I2C_ADDR+1);//�����豸��ַ+���ź�
		IIC_Wait_Ack();
		for(j=0;j<14;j++)
		{
			if(j==13)buf[j]=IIC_Read_Byte(0);//��ȡһ���ֽ�,�������ٶ�,����NACK  
			else buf[j]=IIC_Read_Byte(1);	//��ȡһ���ֽ�,������,����ACK 
		}
		IIC_Stop();					//����һ��ֹͣ����
 		/*����ԭʼ���ݵ�У׼*/
		gyro[0]  = (signed short int)((buf[8]<<8)+buf[9]);
 		gyro[1]  = (signed short int)((buf[10]<<8)+buf[11]);
 		gyro[2]  = (signed short int)((buf[12]<<8)+buf[13]);
 		gyro_x 	+= gyro[0];
 		gyro_y	+= gyro[1];
 		gyro_z	+= gyro[2];
 		count++;
 	}
  	
  	Gyro_Xout = (short int)gyro_x / count;
  	Gyro_Yout = (short int)gyro_y / count;
  	Gyro_Zout = (short int)gyro_z / count;

	//SJS_iNEMO_Init();
	
  	return 0;
}

typedef struct _SJS_SENSOR_DATA{
	s16 acc_x;
	s16 acc_y;
	s16 acc_z;

}SJS_SENSOR_DATA;

SJS_SENSOR_DATA sensor_data;




void GetMPU9250Data(void)
{
	s16 buf[20];
	u8 i;
/*------------------------------------------------------------------------------------------------------------------------------------*/	
//��ȡ���ٶȡ������Ǵ�����
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(MPU9250_I2C_ADDR);  //�����豸��ַ+д�ź�
	IIC_Wait_Ack();	   
	IIC_Send_Byte(MPU9250_ACCEL_XOUT_H);   //���ʹ洢��Ԫ��ַ����0��ʼ
	IIC_Wait_Ack();	   
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(MPU9250_I2C_ADDR+1);//�����豸��ַ+���ź�
	IIC_Wait_Ack();
	for(i=0;i<14;i++)
	{
		if(i==13)buf[i]=IIC_Read_Byte(0);//��ȡһ���ֽ�,�������ٶ�,����NACK  
		else buf[i]=IIC_Read_Byte(1);	//��ȡһ���ֽ�,������,����ACK 
 	}
	IIC_Stop();					//����һ��ֹͣ����
	
/*-----------------------------------------------------------------------------------------------------------------------------------*/	
//��ȡ�شŴ�����
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(AK8963_I2C_ADDR);  //�����豸��ַ+д�ź�
	IIC_Wait_Ack();	   
	IIC_Send_Byte(AK8963_HXL);   //���ʹ洢��Ԫ��ַ����0��ʼ
	IIC_Wait_Ack();	   
	IIC_Start();                  //��ʼ�ź�
	IIC_Send_Byte(AK8963_I2C_ADDR+1);//�����豸��ַ+���ź�
	IIC_Wait_Ack();
	for(i=14;i<20;i++)
	{
		if(i==19)buf[i]=IIC_Read_Byte(0);//��ȡһ���ֽ�,�������ٶ�,����NACK  
		else buf[i]=IIC_Read_Byte(1);	//��ȡһ���ֽ�,������,����ACK 
 	}
	IIC_Stop();					//����һ��ֹͣ����
	
	IIC_Write_OneByte(AK8963_I2C_ADDR,AK8963_CNTL1,0x11);//ÿ��һ�����ݣ�ak8963���Զ�����powerdownģʽ,������Ҫ�����趨Ϊ������ģʽ
/*-------------------------------------------------------------------------------------------------------------------------------------*/		
//���ٶ�
	mpu9250.acc_x=(buf[0]<<8)+buf[1];
	mpu9250.acc_y=(buf[2]<<8)+buf[3];
	mpu9250.acc_z=(buf[4]<<8)+buf[5];
//�¶�
	mpu9250.temp =(buf[6]<<8)+buf[7];
//������
	mpu9250.gyro_x=(buf[8]<<8)+buf[9];
	mpu9250.gyro_y=(buf[10]<<8)+buf[11];
	mpu9250.gyro_z=(buf[12]<<8)+buf[13];
//�شŴ�����
	mpu9250.mag_x=(buf[15]<<8)+buf[14];
	mpu9250.mag_y=(buf[17]<<8)+buf[16];
	mpu9250.mag_z=(buf[19]<<8)+buf[18];
	
	gx = (float) ((mpu9250.gyro_x - Gyro_Xout)*  GYRO_KEN);
	gy = (float) ((mpu9250.gyro_y - Gyro_Yout)*  GYRO_KEN);
	gz = (float) ((mpu9250.gyro_z - Gyro_Zout)*  GYRO_KEN);
	
	gx = gx / 57.3;
	gy = gy / 57.3;
	gz = gz / 57.3;
	
	ax = (float) mpu9250.acc_x ;  
	ay = (float) mpu9250.acc_y ; 
	az = (float) mpu9250.acc_z ; 
	
	ax = ax * ACC_KEN;
	ay = ay * ACC_KEN;
	az = az * ACC_KEN;
	
	mx = (float) mpu9250.mag_x ; 
	my = (float) mpu9250.mag_y ; 
	mz = (float) mpu9250.mag_z; 
	
	mx1 = my;
	my1 = mx;
	mz1 = -mz;
	
	mx1 = mx1 - 162;
	my1 = my1 + 75;
	mz1 = mz1;


	delay_ms(10);

	
	us_mcu_rc_buff_enter(SNS_MCU_SENSOR_WRITE, (unsigned char *)&mpu9250, sizeof(SJSMPU));

	/**Send Attitude**/
	//SJS_iNEMO_Update(mpu9250);



	//printf("ax = %d,ay = %d,az = %d\r\n",mpu9250.acc_x,mpu9250.acc_y,mpu9250.acc_z);
	//printf("gx = %f,gy = %f,gz = %f\r\n",gx,gy,gz);


	
//printf("mx = %f,y = %f,mz = %f\r\n",mx1,my1,mz1);
//	UserToPMABufferCopy(packet_buf, ENDP2_TXADDR, 63);
 //   _SetEPTxStatus(ENDP2, EP_TX_VALID);

}




