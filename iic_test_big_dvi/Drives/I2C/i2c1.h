/*******************************************************************************

 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2016-02-29    �����ˣ�gm
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#ifndef __I2C1_H
#define __I2C1_H
#include "stm32f10x.h"
#include "bitband.h"
#include "i2c.h"

#define I2C1_SPEED_1K		5000	//���ݴ������ٶ����ã����ﴦ�����ٶ���72MHz

//I2C�˿ڶ���//i2c2;
#define I2C1_SCL    GPIOout(GPIOB, 6)	//SCL
#define I2C1_SDA    GPIOout(GPIOB, 7)	//SDA	 
#define READ1_SDA   GPIOin( GPIOB, 7)	//����SDA

//����PB7�������
#define SDA1_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=0X80000000;}
#define SDA1_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=3<<28;}

//extern I2C_StatusTypeDef;

//extern uint32_t i2c_speed;	//I2C�����ٶ� = I2C_SPEED_1K / i2c_speed

/* ---------------------------����I2CЭ���д��ʱ����------------------------------*/
void I2C1_Init(void);				//��ʼ��I2C��IO��				 
//void I2C1_Start(void);				//����I2C��ʼ�ź�
//void I2C_Stop(void);				//����I2Cֹͣ�ź�
//uint8_t I2C_Wait_ACK(void);	//I2C�ȴ�ACK�ź�
//void I2C_ACK(void);					//I2C����ACK�ź�
//void I2C_NACK(void);				//I2C������ACK�ź�
//void I2C_Send_Byte(uint8_t data);		//I2C����һ���ֽ�
//uint8_t I2C_Read_Byte(uint8_t ack);	//I2C��ȡһ���ֽ�
//uint16_t I2C1_SetSpeed(uint16_t speed);//����I2C�ٶ�(1Kbps~400Kbps,speed��λ��Kbps)

/* ---------------------------���²����Ƿ�װ�õ�I2C��д����--------------------------- */

//���嵽ĳһ������������ϸ�Ķ�������������I2C���ֵ�˵������ΪĳЩ������I2C�Ķ�д������
//��һЩ���죬����Ĵ��������ھ��������I2C�����У�������֤OK�ģ�
I2C_StatusTypeDef I2C1_WriteOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t Data);//��I2C���豸д��һ���ֽ�
//I2C_StatusTypeDef I2C_WriteBurst(uint8_t DevAddr, uint8_t DataAddr, uint8_t* pData, uint32_t Num);//��I2C���豸����д��Num���ֽ�
I2C_StatusTypeDef I2C1_ReadOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t* Data);//��I2C���豸��ȡһ���ֽ�
//I2C_StatusTypeDef I2C_ReadBurst(uint8_t DevAddr, uint8_t DataAddr, uint8_t* pData, uint32_t Num);//��I2C�豸������ȡNum���ֽ�
//I2C_StatusTypeDef I2C_WriteBit(uint8_t DevAddr, uint8_t DataAddr, uint8_t Bitx, uint8_t BitSet);

#endif

/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/

