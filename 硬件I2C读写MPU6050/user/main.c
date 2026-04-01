#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址
/*-----------MPU6050寄存器地址-------------*/
#define	MPU6050_SMPLRT_DIV		0x19
#define	MPU6050_CONFIG			0x1A
#define	MPU6050_GYRO_CONFIG		0x1B
#define	MPU6050_ACCEL_CONFIG	0x1C

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75
/*-----------MPU6050寄存器地址-------------*/

/*------------MPU6050-------------*/
void mpu6050_init(void);//初始化
void mpu6050_writereg(uint8_t regrddress,uint8_t data);//对指定地址写
uint8_t mpu6050_readreg(uint8_t regrddress);//对指定地址读
struct acc_and_gyro//用于返回多个参数
{
	int16_t accx;
	int16_t accy;
	int16_t accz;
	int16_t gyrox;
	int16_t gyroy;
	int16_t gyroz;
};
void mpu6050_getdata(struct acc_and_gyro *ag);
void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
/*------------MPU6050-------------*/

int main(void)
{
	OLED_Init(); 

	mpu6050_init();
	
	struct acc_and_gyro ag;
	
	while(1)
	{
		mpu6050_getdata(&ag);
		OLED_ShowSignedNum(2,1,ag.accx,5);
		OLED_ShowSignedNum(3,1,ag.accy,5);
		OLED_ShowSignedNum(4,1,ag.accz,5);
		OLED_ShowSignedNum(2,8,ag.gyrox,5);
		OLED_ShowSignedNum(3,8,ag.gyroy,5);
		OLED_ShowSignedNum(4,8,ag.gyroz,5);
	}
}

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)
{
	uint32_t Timeout;
	Timeout = 10000;									//给定超时计数时间
	while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)	//循环等待指定事件
	{
		Timeout --;										//等待时，计数值自减
		if (Timeout == 0)								//自减到0后，等待超时
		{
			/*超时的错误处理代码，可以添加到此处*/
			break;										//跳出等待，不等了
		}
	}
}

void mpu6050_getdata(struct acc_and_gyro *ag)
{
	uint8_t dh,dl;
	
	dh = mpu6050_readreg(MPU6050_ACCEL_XOUT_H);
	dl = mpu6050_readreg(MPU6050_ACCEL_XOUT_L);
	ag->accx = (dh << 8) | dl;
	
	dh = mpu6050_readreg(MPU6050_ACCEL_YOUT_H);
	dl = mpu6050_readreg(MPU6050_ACCEL_YOUT_L);
	ag->accy = (dh << 8) | dl;
	
	dh = mpu6050_readreg(MPU6050_ACCEL_ZOUT_H);
	dl = mpu6050_readreg(MPU6050_ACCEL_ZOUT_L);
	ag->accz = (dh << 8) | dl;
	
	dh = mpu6050_readreg(MPU6050_GYRO_XOUT_H);
	dl = mpu6050_readreg(MPU6050_GYRO_XOUT_L);
	ag->gyrox = (dh << 8) | dl;
	
	dh = mpu6050_readreg(MPU6050_GYRO_YOUT_H);
	dl = mpu6050_readreg(MPU6050_GYRO_YOUT_L);
	ag->gyroy = (dh << 8) | dl;
	
	dh = mpu6050_readreg(MPU6050_GYRO_ZOUT_H);
	dl = mpu6050_readreg(MPU6050_GYRO_ZOUT_L);
	ag->gyroz = (dh << 8) | dl;
}

uint8_t mpu6050_readreg(uint8_t regrddress)
{
	uint8_t data;
	
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	//硬件I2C发送从机地址，方向为发送
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, regrddress);											//硬件I2C发送寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成重复起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Receiver);		//硬件I2C发送从机地址，方向为接收
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);		//等待EV6
	
	I2C_AcknowledgeConfig(I2C2, DISABLE);									//在接收最后一个字节之前提前将应答失能
	I2C_GenerateSTOP(I2C2, ENABLE);											//在接收最后一个字节之前提前申请停止条件
	
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);				//等待EV7
	data = I2C_ReceiveData(I2C2);											//接收数据寄存器
	
	I2C_AcknowledgeConfig(I2C2, ENABLE);									//将应答恢复为使能，为了不影响后续可能产生的读取多字节操作
	
	return data;
}

void mpu6050_writereg(uint8_t regrddress,uint8_t data)
{
	I2C_GenerateSTART(I2C2, ENABLE);										//硬件I2C生成起始条件
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);					//等待EV5
	
	I2C_Send7bitAddress(I2C2, MPU6050_ADDRESS, I2C_Direction_Transmitter);	//硬件I2C发送从机地址，方向为发送
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	//等待EV6
	
	I2C_SendData(I2C2, regrddress);											//硬件I2C发送寄存器地址
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING);			//等待EV8
	
	I2C_SendData(I2C2, data);												//硬件I2C发送数据
	MPU6050_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);				//等待EV8_2
	
	I2C_GenerateSTOP(I2C2, ENABLE);											//硬件I2C生成终止条件
}

void mpu6050_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	I2C_InitTypeDef I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;				//模式，选择为I2C模式
	I2C_InitStructure.I2C_ClockSpeed = 50000;				//时钟速度，选择为50KHz
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;		//时钟占空比，选择Tlow/Thigh = 2
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;				//应答，选择使能
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;	//应答地址，选择7位，从机模式下才有效
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;				//自身地址，从机模式下才有效
	I2C_Init(I2C2, &I2C_InitStructure);						//将结构体变量交给I2C_Init，配置I2C2
	
	I2C_Cmd(I2C2, ENABLE);
	
	mpu6050_writereg(MPU6050_PWR_MGMT_1, 0x01);		//解除睡眠
	mpu6050_writereg(MPU6050_PWR_MGMT_2, 0x00);		//选择陀螺仪时钟 6个轴均不待机
	mpu6050_writereg(MPU6050_SMPLRT_DIV, 0x09);		//配置采样率，0x09为10
	mpu6050_writereg(MPU6050_CONFIG, 0x06);			//配置滤波参数
	mpu6050_writereg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	mpu6050_writereg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}