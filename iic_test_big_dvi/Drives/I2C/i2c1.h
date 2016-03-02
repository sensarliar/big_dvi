/*******************************************************************************

 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * 更改时间：2016-02-29    更改人：gm
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#ifndef __I2C1_H
#define __I2C1_H
#include "stm32f10x.h"
#include "bitband.h"
#include "i2c.h"

#define I2C1_SPEED_1K		5000	//根据处理器速度设置，这里处理器速度是72MHz

//I2C端口定义//i2c2;
#define I2C1_SCL    GPIOout(GPIOB, 6)	//SCL
#define I2C1_SDA    GPIOout(GPIOB, 7)	//SDA	 
#define READ1_SDA   GPIOin( GPIOB, 7)	//输入SDA

//设置PB7输入输出
#define SDA1_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X80000000;}
#define SDA1_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}

//extern I2C_StatusTypeDef;

//extern uint32_t i2c_speed;	//I2C访问速度 = I2C_SPEED_1K / i2c_speed

/* ---------------------------依照I2C协议编写的时序函数------------------------------*/
void I2C1_Init(void);				//初始化I2C的IO口				 
//void I2C1_Start(void);				//发送I2C开始信号
//void I2C_Stop(void);				//发送I2C停止信号
//uint8_t I2C_Wait_ACK(void);	//I2C等待ACK信号
//void I2C_ACK(void);					//I2C发送ACK信号
//void I2C_NACK(void);				//I2C不发送ACK信号
//void I2C_Send_Byte(uint8_t data);		//I2C发送一个字节
//uint8_t I2C_Read_Byte(uint8_t ack);	//I2C读取一个字节
//uint16_t I2C1_SetSpeed(uint16_t speed);//设置I2C速度(1Kbps~400Kbps,speed单位，Kbps)

/* ---------------------------以下部分是封装好的I2C读写函数--------------------------- */

//具体到某一个器件，请仔细阅读器件规格书关于I2C部分的说明，因为某些器件在I2C的读写操作会
//有一些差异，下面的代码我们在绝大多数的I2C器件中，都是验证OK的！
I2C_StatusTypeDef I2C1_WriteOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t Data);//向I2C从设备写入一个字节
//I2C_StatusTypeDef I2C_WriteBurst(uint8_t DevAddr, uint8_t DataAddr, uint8_t* pData, uint32_t Num);//向I2C从设备连续写入Num个字节
I2C_StatusTypeDef I2C1_ReadOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t* Data);//从I2C从设备读取一个字节
//I2C_StatusTypeDef I2C_ReadBurst(uint8_t DevAddr, uint8_t DataAddr, uint8_t* pData, uint32_t Num);//从I2C设备连续读取Num个字节
//I2C_StatusTypeDef I2C_WriteBit(uint8_t DevAddr, uint8_t DataAddr, uint8_t Bitx, uint8_t BitSet);

#endif

/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/

