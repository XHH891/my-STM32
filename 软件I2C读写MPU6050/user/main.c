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
/*------------MPU6050-------------*/

/*------------I2C相关-------------*/
void myi2c_start(void);//通信开始
void myi2c_stop(void);//通信结束
void myi2c_sendack(uint8_t ackbit);//发送应答
uint8_t myi2c_receiveack(void);//接收应答
void myi2c_sendbyte(uint16_t byte);//发送一字节数据
uint8_t myi2c_receivebyte(void);//接收一字节数据

void myi2c_w_scl(uint8_t bitvalue);//对SCL写，输入1为高电平，0为低电平，方便操作
void myi2c_w_sda(uint8_t bitvalue);//对SDA写，输入1为高电平，0为低电平，方便操作
uint8_t myi2c_r_sda(void);//读取SDA线的电平
void myi2c_init(void);
/*------------I2C相关-------------*/

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
	
	myi2c_start();
	myi2c_sendbyte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	myi2c_receiveack();					//接收应答
	myi2c_sendbyte(regrddress);			//发送寄存器地址
	myi2c_receiveack();					//接收应答
	
	myi2c_start();
	myi2c_sendbyte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为0，表示即将写入
	myi2c_receiveack();					//接收应答
	data = myi2c_receivebyte();//接收数据
	myi2c_sendack(1);//发送应答，0为收到应答，从机继续发送，1为不回复应答，主机收回控制权
	myi2c_stop();						//I2C终止
	
	return data;
}

void mpu6050_writereg(uint8_t regrddress,uint8_t data)
{
	myi2c_start();						//I2C起始
	myi2c_sendbyte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	myi2c_receiveack();					//接收应答
	myi2c_sendbyte(regrddress);			//发送寄存器地址
	myi2c_receiveack();					//接收应答
	myi2c_sendbyte(data);				//发送要写入寄存器的数据
	myi2c_receiveack();					//接收应答
	myi2c_stop();						//I2C终止
}

void mpu6050_init(void)
{
	myi2c_init();
	
	mpu6050_writereg(MPU6050_PWR_MGMT_1, 0x01);		//解除睡眠
	mpu6050_writereg(MPU6050_PWR_MGMT_2, 0x00);		//选择陀螺仪时钟 6个轴均不待机
	mpu6050_writereg(MPU6050_SMPLRT_DIV, 0x09);		//配置采样率，0x09为10
	mpu6050_writereg(MPU6050_CONFIG, 0x06);			//配置滤波参数
	mpu6050_writereg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	mpu6050_writereg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

uint8_t myi2c_receiveack(void)
{
	uint8_t ackbit;
	myi2c_w_sda(1);
	myi2c_w_scl(1);
	ackbit = myi2c_r_sda();
	myi2c_w_scl(0);
	return ackbit;
}

void myi2c_sendack(uint8_t ackbit)
{
	myi2c_w_sda(ackbit);
	myi2c_w_scl(1);
	myi2c_w_scl(0);
}

void myi2c_start(void)
{
	myi2c_w_sda(1);
	myi2c_w_scl(1);
	myi2c_w_sda(0);
	myi2c_w_scl(0);
}

void myi2c_stop(void)
{
	myi2c_w_sda(0);
	myi2c_w_scl(1);
	myi2c_w_sda(1);
}

void myi2c_sendbyte(uint16_t byte)
{
	uint8_t i;
	for(i=0;i<8;i++)
	{
		myi2c_w_sda(byte & (0x80 >> i));
		myi2c_w_scl(1);
		myi2c_w_scl(0);
	}
}

uint8_t myi2c_receivebyte(void)
{
	uint8_t i,byte = 0x00;
	myi2c_w_sda(1);
	for(i=0;i<8;i++)
	{
		myi2c_w_scl(1);
		if(myi2c_r_sda() == 1)
		{
			byte |= (0x80 >> i);
		}
		myi2c_w_scl(0);
	}
	return byte;
}

uint8_t myi2c_r_sda(void)
{
	uint8_t bitvalue;
	bitvalue = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
	Delay_us(10);
	return bitvalue;
}

void myi2c_w_sda(uint8_t bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_11,(BitAction)bitvalue);
	Delay_us(10);
}

void myi2c_w_scl(uint8_t bitvalue)
{
	GPIO_WriteBit(GPIOB,GPIO_Pin_10,(BitAction)bitvalue);
	Delay_us(10);
}

void myi2c_init(void)//I2C线路初始化
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Speed =GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	GPIO_SetBits(GPIOB,GPIO_Pin_10 | GPIO_Pin_11);
}