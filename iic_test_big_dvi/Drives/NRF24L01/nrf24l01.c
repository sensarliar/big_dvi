/*******************************************************************************
 * @name    : 2.4G无线模块NRF24L01驱动
 * @author  : 布谷鸟
 * @web     : WWW.UCORTEX.COM
 * @version : V1.0
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : 2.4G无线模块驱动
 * ---------------------------------------------------------------------------- 
 * @copyright
 *
 * UCORTEX版权所有，违者必究！例程源码仅供大家参考，旨在交流学习和节省开发时间，
 * 对于因为参考本文件内容，导致您的产品直接或间接受到破坏，或涉及到法律问题，作
 * 者不承担任何责任。对于使用过程中发现的问题，如果您能在WWW.UCORTEX.COM网站反
 * 馈给我们，我们会非常感谢，并将对问题及时研究并作出改善。例程的版本更新，将不
 * 做特别通知，请您自行到WWW.UCORTEX.COM下载最新版本，谢谢。
 * 对于以上声明，UCORTEX保留进一步解释的权利！
 * ----------------------------------------------------------------------------
 * @description
 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * 更改时间：2014-04-06    更改人：布谷鸟
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
 
#include "nrf24l01.h"
#include "spi.h"
#include "delay.h"

/**
  * @brief NRF24L01无线模块初始化
  * @param None
  * @retval none
  * @note
	* 	NRF24L01无线模块连接到SPI1上
	*		NRF_CE: 	PA2
	*		NRF_CSN:	PA3
	*		NRF_IRQ:	PA4
  */
void NRF24L01_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//配置NRF24L01片选管脚和SPI选通管脚,片选: PA2; SPI选通: PA3
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//根据上面配置信息初始化
	NRF_CSN_DESELECT();//初始化SPI之前，先使NRF24L01处于非选通状态
/*	
	delay_ms(1);
		NRF_CSN_SELECT();//初始化SPI之前，先使NRF24L01处于非选通状态
		delay_ms(1);
		NRF_CSN_DESELECT();//初始化SPI之前，先使NRF24L01处于非选通状态
	*/
	//配置NRF24L01中断引脚为下拉输入
	/*
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/
	SPI1_Init();	//SPI1初始化
	
//	NRF_CE = 0;	//NRF24L01片选使能
}

/**
  * @brief 写寄存器函数
  * @param reg : 寄存器地址
	* @param val : 设置的寄存器值
  * @retval NRF24L01操作状态
  * @note
	* 	
  */
NRF_StatusTypedef NRF24L01_Write_Reg(uint8_t reg, uint8_t val)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
		
	NRF_CSN_SELECT();	//获取SPI总线
	timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
	status = SPI_RW(NRF_SPI,reg,&timeout);//写寄存器地址
	if(timeout == 0x00 && status == 0xff)//判断是否超时
	{
		NRF_CSN_DESELECT();	//释放SPI总选
		return NRF_TIMEOUT;		//返回错误
	}
	
	timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
	status = SPI_RW(NRF_SPI,	val, &timeout);//写数据
	if(timeout == 0x00 && status == 0xff)//判断是否超时
	{
		NRF_CSN_DESELECT();//释放SPI总选
		return NRF_TIMEOUT;//返回错误
	}
	
	NRF_CSN_DESELECT();//释放SPI总选
	
	return NRF_SUCCESS;//返回操作成功
}

/**
  * @brief 读寄存器
  * @param reg : 寄存器地址
	* @param *val : 读取到的寄存器值存放地址
  * @retval NRF24L01操作状态
  * @note
	* 	
  */
NRF_StatusTypedef NRF24L01_Read_Reg(uint8_t reg, uint8_t* val)
{
	uint8_t data = 0;
	uint32_t timeout = 0;
		
	NRF_CSN_SELECT();	//获取SPI总线
	timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
	data = SPI_RW(NRF_SPI,0x00,&timeout);//写寄存器地址
	if(timeout == 0x00 && data == 0xff)//判断是否超时
	{
		NRF_CSN_DESELECT();	//释放SPI总选
		return NRF_TIMEOUT;		//返回错误
	}
	
	*val = data;				//保存读取的寄存器值
	NRF_CSN_DESELECT();	//释放SPI总选
	
	return NRF_SUCCESS;	//返回操作成功
}

/**
  * @brief 连续写数据到缓冲区(NRF24L01 FIFO)
  * @param reg : 寄存器地址
	* @param *pbuf : 数据包首地址
	* @param num : 发送的数据个数
  * @retval NRF24L01操作状态
  * @note  该函数主要用于将一组数据放到NRF24L01的发射FIFO缓冲区
	* 	
  */
NRF_StatusTypedef NRF24L01_Write_Buf(uint8_t reg, uint8_t* pbuf, uint8_t num)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t i = 0;
	
	NRF_CSN_SELECT();//获取SPI总线
	timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
	status = SPI_RW(NRF_SPI,reg,&timeout);	//选择要写入的寄存器
	if(timeout == 0x00 && status == 0xff)//判断是否超时
	{
		NRF_CSN_DESELECT();	//释放SPI总选
		return NRF_TIMEOUT;	//返回错误
	}
	
	for(i = 0; i < num; i++)
	{
		timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
		status = SPI_RW(NRF_SPI,*pbuf,&timeout);
		if(timeout == 0x00 && status == 0xff)//判断是否超时
		{
			NRF_CSN_DESELECT();	//释放SPI总选
			return NRF_TIMEOUT;	//返回错误
		}
		pbuf++;
	}
	NRF_CSN_DESELECT();//释放SPI总线
	return NRF_SUCCESS;//返回操作成功
}

/**
  * @brief 连续写数据到缓冲区(NRF24L01 FIFO)
  * @param reg : 寄存器地址
	* @param *pbuf : 数据包首地址
	* @param num : 发送的数据个数
  * @retval NRF24L01操作状态
  * @note  该函数主要用于将一组数据放到NRF24L01的发射FIFO缓冲区
	* 	
  */
NRF_StatusTypedef NRF24L01_Read_Buf(uint8_t reg, uint8_t* pbuf, uint8_t num)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t i = 0;
	
	NRF_CSN_SELECT();//获取SPI总线
	timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
	status = SPI_RW(NRF_SPI,reg,&timeout);	//选择要写入的寄存器
	if(timeout == 0x00 && status == 0xff)//判断是否超时
	{
		NRF_CSN_DESELECT();	//释放SPI总选
		return NRF_TIMEOUT;	//返回错误
	}
	
	for(i = 0; i < num; i++)
	{
		timeout = NRF_TIMEOUT_VAL;//设置访问超时时间
		status = SPI_RW(NRF_SPI,0x00,&timeout);
		if(timeout == 0x00 && status == 0xff)//判断是否超时
		{
			NRF_CSN_DESELECT();	//释放SPI总选
			return NRF_TIMEOUT;	//返回错误
		}
		*pbuf = status;//存放数据
		pbuf++;
	}
	
	NRF_CSN_DESELECT();//释放SPI总线
	return NRF_SUCCESS;//返回操作成功
}

/**
  * @brief 连续写数据到缓冲区(NRF24L01 FIFO)
  * @param reg : 寄存器地址
	* @param *pbuf : 数据包首地址
	* @param num : 发送的数据个数
  * @retval NRF24L01操作状态
  * @note  该函数主要用于将一组数据放到NRF24L01的发射FIFO缓冲区
	* 	
  */
NRF_StatusTypedef A429_config(void)
{
	NRF_StatusTypedef status = NRF_ERROR;
	uint8_t test[5] = {0x06,0xBA,0xBA,0xBA,0x0C};
	
		delay_ms(10);
	//将测试数据写入发送缓冲
	//status = NRF24L01_Write_Buf(NRF_WRITE_REG|TX_ADDR, test, 1);WR_R1_CTL
	status = NRF24L01_Write_Buf(WR_R1_CTL, test, 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误
	
		status = NRF24L01_Write_Buf(WR_R1_PL1_MATCH, &test[1], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误
	
			status = NRF24L01_Write_Buf(WR_FLAG_ASSIGN, &test[4], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误
	
		return NRF_SUCCESS;	//NRF24L01通信正常
}

NRF_StatusTypedef NRF24L01_Check(void)
{
	NRF_StatusTypedef status = NRF_ERROR;
	uint8_t result[20] = {0x00,0x00,0x00,0x00,0x00};
	uint8_t i = 0;
//	uint8_t display_staxx =0;
	
	
	//	status = NRF24L01_Write_Buf(MASTER_RST, test, 1);

	
	//从发送缓冲读取数据
	//status = NRF24L01_Read_Buf(NRF_READ_REG|TX_ADDR, result, 1);
	status = NRF24L01_Read_Buf(RD_R1_CTL, result, 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误
	
	status = NRF24L01_Read_Buf(RD_R1_STA, &result[1], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误	

	status = NRF24L01_Read_Buf(RD_R1_FIFO, &result[2], 4);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误		

		status = NRF24L01_Read_Buf(RD_R1_PL1_CONTENT, &result[6], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误		
	

	
	
			status = NRF24L01_Read_Buf(RD_R1_PL1_MATCH, &result[9], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信
	
		status = NRF24L01_Read_Buf(RD_FLAG_ASSIGN, &result[12], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01通信错误
	
			status = NRF24L01_Read_Buf(RD_FLAG_ASSIGN, &result[15], 1);
	if(status != NRF_SUCCESS)return status;
		if(0x30==result[8])
			return NRF_ERROR;
			
	/*
	//判断读取到的数据是否和发送的数据相同
	for(i = 0; i < 1; i++)
	{
		if(result[i] != test[i])
			return NRF_ERROR;//数据错误
	}
	
	//display_sta = (result[8]&0x30)>>4;
	display_sta = result[8];
	if(display_sta==1)
	{
		status = NRF_ERROR;
	}
	else if(display_sta==2)
	{
		status = NRF_SUCCESS;
	}	
*/	
	
	
	
	return NRF_SUCCESS;	//NRF24L01通信正常
	
}


/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/
