/*******************************************************************************
 * @name    : 串口通信底层驱动
 * @author  : 布谷鸟
 * @web     : WWW.UCORTEX.COM
 * @version : V1.1
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : STM32串口的配置和使用
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
 * 注意使用本串口驱动文件时，需要在stm32f10x_it.c编写相应的串口接收中断函数
 * void USART1_IRQHandler(void)。
 *
 * COM1:
 * USART1_TX -> PA9
 * USART1_RX -> PA10
 * 
 * COM2:
 * USART2_TX -> PA2
 * USART2_RX -> PA3
 * 
 * COM3:
 * USART3_TX -> PB10
 * USART3_RX -> PB11
 *
 * LED -> PB12
 * KEY -> PA0
 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * 更改时间：2014-04-03    更改人：布谷鸟
 * 版本记录：V1.0
 * 更改内容：新建
 * ----------------------------------------------------------------------------
 * 更改时间：2014-05-01    更改人：布谷鸟
 * 版本记录：V1.1
 * 更改内容：添加了DMA传输方式的支持
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "usart.h"
#if COM_DMA_TRANSFER
#include "dma.h"
#endif

#if COM_RX_LEN
/* COM_RX_STA: 串口接收状态字
 * bit15，	接收完成标志
 * bit14，	接收到0x0d
 * bit13~0，	接收到的有效字节数目
*/
uint16_t COM_RX_STA = 0;

/* 串口接收缓冲 */
uint8_t COM_RX_BUF[COM_RX_LEN];
#endif

/******************************* @printf  *************************************/

#ifdef PRINTF_SUPPORT //以下代码,支持printf函数,而不需要选择use MicroLIB
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
int _sys_exit(int x) 
{ 
	return x;
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(PRINTF_SUPPORT, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(PRINTF_SUPPORT, USART_FLAG_TC) == RESET)
  {}    
	return ch;
}
#endif
/********************************* @end ***************************************/

/**
  * @brief 定义串口相关PORT，CLK等
  */
USART_TypeDef* COM_USART[COMn] = {UCORTEX_COM1, UCORTEX_COM2, UCORTEX_COM3}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {UCORTEX_COM1_TX_GPIO_PORT, UCORTEX_COM2_TX_GPIO_PORT, UCORTEX_COM3_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {UCORTEX_COM1_RX_GPIO_PORT, UCORTEX_COM2_RX_GPIO_PORT, UCORTEX_COM3_RX_GPIO_PORT};
 
const uint32_t COM_USART_CLK[COMn] = {UCORTEX_COM1_CLK, UCORTEX_COM2_CLK, UCORTEX_COM3_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {UCORTEX_COM1_TX_GPIO_CLK, UCORTEX_COM2_TX_GPIO_CLK, UCORTEX_COM3_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {UCORTEX_COM1_RX_GPIO_CLK, UCORTEX_COM2_RX_GPIO_CLK, UCORTEX_COM3_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {UCORTEX_COM1_TX_PIN, UCORTEX_COM2_TX_PIN, UCORTEX_COM3_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {UCORTEX_COM1_RX_PIN, UCORTEX_COM2_RX_PIN, UCORTEX_COM3_RX_PIN};

const uint8_t COM_IRQn[COMn] = {UCORTEX_COM1_IRQn, UCORTEX_COM2_IRQn, UCORTEX_COM3_IRQn};

#if COM_DMA_TRANSFER
DMA_TypeDef* COM_DMA[COMn] = {UCORTEX_COM1_DMA, UCORTEX_COM2_DMA, UCORTEX_COM3_DMA}; 

DMA_Channel_TypeDef* COM_DMA_CH[COMn][2] = {{UCORTEX_COM1_RX_DMA_CH, UCORTEX_COM1_TX_DMA_CH},
																						{UCORTEX_COM2_RX_DMA_CH, UCORTEX_COM2_TX_DMA_CH},
																						{UCORTEX_COM3_RX_DMA_CH, UCORTEX_COM3_TX_DMA_CH}};
																						
const uint32_t COM_DR_BASE[COMn] = { UCORTEX_COM1_DR_Base, UCORTEX_COM2_DR_Base, UCORTEX_COM3_DR_Base};

#endif


/**
  * @brief  初始化串口GPIO
  * @param  COM: 指定要初始化的COM PORT   
  *     @arg COM1 串口1
  *     @arg COM2 串口2
  *     @arg COM3 串口3
  * @retval None
  * @note None
  */
void COM_GPIOInit(COM_TypeDef COM)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 开启GPIO时钟和复用功能时钟RCC_APB2Periph_AFIO */
  RCC_APB2PeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM] | RCC_APB2Periph_AFIO, ENABLE);

  /* 配置 USART Tx 复用推挽输出 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);

  /* 配置 USART Rx 浮空输入 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);
}

/**
  * @brief  配置串口
  * @param  COM: 指定要初始化的COM PORT   
  *     @arg COM1 串口1
  *     @arg COM2 串口2
  *     @arg COM3 串口3
  * @param  Baudrate 串口波特率
  * @retval None
  * @note None
  */
void COM_Init(COM_TypeDef COM, uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
  COM_GPIOInit(COM);//初始化串口GPIO

  /* 开启串口时钟，注意串口1挂载在APB1外设总线下，其他串口挂载APB2外设总线下*/
  if (COM == COM1)
    RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); //开启串口时钟
  else
    RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);	//开启串口时钟
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置优先级组2
	/* USARTx NVIC配置信息 */
	NVIC_InitStructure.NVIC_IRQChannel = COM_IRQn[COM];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;				//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据上面的配置信息初始化NVIC寄存器
	
	
  /*    串口配置信息
	      -------------
        - 波特率根据参数指定
        - 数据长度8bit
        - 1个停止位
        - 无奇偶校验位
        - 无硬件数据流控制(RTS 和 CTS 信号)
        - 允许串口发送和接收
  */
  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(COM_USART[COM], &USART_InitStructure);			//根据串口配置信息初始化串口 

	USART_ITConfig(COM_USART[COM], USART_IT_RXNE, ENABLE);//开启串口接收中断
  USART_Cmd(COM_USART[COM], ENABLE);										//开启串口
}

#if COM_DMA_TRANSFER
/**
  * @brief  轮询方式(Polling)串口DMA初始化
  * @param  COM: 要设置的串口 
  *     @arg COM1 串口1(USART1)
  *     @arg COM2 串口2(USART2)
  *     @arg COM3 串口3(USART3)
  * @param  MemoryBaseAddr : DMA缓冲地址（一般是我们定义的缓冲数组地址）
	* @param  BufferSize : 缓冲大小
	* @param  DMA_DIR : DMA传输方向
  *     @arg DMA_DIR_PeripheralDST : 串口发送DMA，即 Buffer->USART 的DMA传输
  *     @arg DMA_DIR_PeripheralSRC : 串口接收DMA，即 USART->Buffer 的DMA传输
  * @retval None
  * @note 在本函数中，打开了相应的串口DMA接口，但并未启动DMA传输，当需要启动DMA传输时：
	*				请调用DMA_Polling_Enable(DMA_Channel_TypeDef* DMAy_Channelx, uint16_t DataNumber)
  */
void COM_DMA_Polling_Init(COM_TypeDef COM, uint32_t MemoryBaseAddr, uint32_t BufferSize, uint32_t DMA_DIR)
{
	//串口对应的DMA
	DMA_TypeDef* DMAy = COM_DMA[COM];
	//对应的DMA通道（注意串口接收和发送对应不同的DMA通道）
	DMA_Channel_TypeDef* DMAy_Channelx = COM_DMA_CH[COM][DMA_DIR>>4];
	//串口数据寄存器地址
	uint32_t PeripheralBaseAddr = COM_DR_BASE[COM];
	//数据位宽: 0 -> 8bit, 1 -> 16bit, 2-> 32bit
	uint32_t DataType = 0; //8-bit位宽
	
	//步骤一、配置对应的DMA通道
	DMA_Polling_Init(DMAy, DMAy_Channelx, PeripheralBaseAddr, MemoryBaseAddr, BufferSize, DataType, DMA_DIR, DMA_Mode_Normal);
	
	//步骤二、打开串口DMA发送或接收接口
	if(DMA_DIR==DMA_DIR_PeripheralDST)//打开串口DMA发送
		USART_DMACmd(COM_USART[COM], USART_DMAReq_Tx, ENABLE);
	
	if(DMA_DIR==DMA_DIR_PeripheralSRC)//打开串口DMA接收
		USART_DMACmd(COM_USART[COM], USART_DMAReq_Rx, ENABLE);
}

#endif

/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/
