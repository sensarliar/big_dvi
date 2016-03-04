/*******************************************************************************
 * @name    : 2.4G����ģ��NRF24L01����
 * @author  : ������
 * @web     : WWW.UCORTEX.COM
 * @version : V1.0
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : 2.4G����ģ������
 * ---------------------------------------------------------------------------- 
 * @copyright
 *
 * UCORTEX��Ȩ���У�Υ�߱ؾ�������Դ�������Ҳο���ּ�ڽ���ѧϰ�ͽ�ʡ����ʱ�䣬
 * ������Ϊ�ο����ļ����ݣ��������Ĳ�Ʒֱ�ӻ����ܵ��ƻ������漰���������⣬��
 * �߲��е��κ����Ρ�����ʹ�ù����з��ֵ����⣬���������WWW.UCORTEX.COM��վ��
 * �������ǣ����ǻ�ǳ���л�����������⼰ʱ�о����������ơ����̵İ汾���£�����
 * ���ر�֪ͨ���������е�WWW.UCORTEX.COM�������°汾��лл��
 * ��������������UCORTEX������һ�����͵�Ȩ����
 * ----------------------------------------------------------------------------
 * @description
 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2014-04-06    �����ˣ�������
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
 
#include "nrf24l01.h"
#include "spi.h"
#include "delay.h"

/**
  * @brief NRF24L01����ģ���ʼ��
  * @param None
  * @retval none
  * @note
	* 	NRF24L01����ģ�����ӵ�SPI1��
	*		NRF_CE: 	PA2
	*		NRF_CSN:	PA3
	*		NRF_IRQ:	PA4
  */
void NRF24L01_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//����NRF24L01Ƭѡ�ܽź�SPIѡͨ�ܽ�,Ƭѡ: PA2; SPIѡͨ: PA3
	//GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//�������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��������������Ϣ��ʼ��
	NRF_CSN_DESELECT();//��ʼ��SPI֮ǰ����ʹNRF24L01���ڷ�ѡͨ״̬
/*	
	delay_ms(1);
		NRF_CSN_SELECT();//��ʼ��SPI֮ǰ����ʹNRF24L01���ڷ�ѡͨ״̬
		delay_ms(1);
		NRF_CSN_DESELECT();//��ʼ��SPI֮ǰ����ʹNRF24L01���ڷ�ѡͨ״̬
	*/
	//����NRF24L01�ж�����Ϊ��������
	/*
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3;   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	*/
	SPI1_Init();	//SPI1��ʼ��
	
//	NRF_CE = 0;	//NRF24L01Ƭѡʹ��
}

/**
  * @brief д�Ĵ�������
  * @param reg : �Ĵ�����ַ
	* @param val : ���õļĴ���ֵ
  * @retval NRF24L01����״̬
  * @note
	* 	
  */
NRF_StatusTypedef NRF24L01_Write_Reg(uint8_t reg, uint8_t val)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
		
	NRF_CSN_SELECT();	//��ȡSPI����
	timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
	status = SPI_RW(NRF_SPI,reg,&timeout);//д�Ĵ�����ַ
	if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
	{
		NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
		return NRF_TIMEOUT;		//���ش���
	}
	
	timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
	status = SPI_RW(NRF_SPI,	val, &timeout);//д����
	if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
	{
		NRF_CSN_DESELECT();//�ͷ�SPI��ѡ
		return NRF_TIMEOUT;//���ش���
	}
	
	NRF_CSN_DESELECT();//�ͷ�SPI��ѡ
	
	return NRF_SUCCESS;//���ز����ɹ�
}

/**
  * @brief ���Ĵ���
  * @param reg : �Ĵ�����ַ
	* @param *val : ��ȡ���ļĴ���ֵ��ŵ�ַ
  * @retval NRF24L01����״̬
  * @note
	* 	
  */
NRF_StatusTypedef NRF24L01_Read_Reg(uint8_t reg, uint8_t* val)
{
	uint8_t data = 0;
	uint32_t timeout = 0;
		
	NRF_CSN_SELECT();	//��ȡSPI����
	timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
	data = SPI_RW(NRF_SPI,0x00,&timeout);//д�Ĵ�����ַ
	if(timeout == 0x00 && data == 0xff)//�ж��Ƿ�ʱ
	{
		NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
		return NRF_TIMEOUT;		//���ش���
	}
	
	*val = data;				//�����ȡ�ļĴ���ֵ
	NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
	
	return NRF_SUCCESS;	//���ز����ɹ�
}

/**
  * @brief ����д���ݵ�������(NRF24L01 FIFO)
  * @param reg : �Ĵ�����ַ
	* @param *pbuf : ���ݰ��׵�ַ
	* @param num : ���͵����ݸ���
  * @retval NRF24L01����״̬
  * @note  �ú�����Ҫ���ڽ�һ�����ݷŵ�NRF24L01�ķ���FIFO������
	* 	
  */
NRF_StatusTypedef NRF24L01_Write_Buf(uint8_t reg, uint8_t* pbuf, uint8_t num)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t i = 0;
	
	NRF_CSN_SELECT();//��ȡSPI����
	timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
	status = SPI_RW(NRF_SPI,reg,&timeout);	//ѡ��Ҫд��ļĴ���
	if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
	{
		NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
		return NRF_TIMEOUT;	//���ش���
	}
	
	for(i = 0; i < num; i++)
	{
		timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
		status = SPI_RW(NRF_SPI,*pbuf,&timeout);
		if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
		{
			NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
			return NRF_TIMEOUT;	//���ش���
		}
		pbuf++;
	}
	NRF_CSN_DESELECT();//�ͷ�SPI����
	return NRF_SUCCESS;//���ز����ɹ�
}

/**
  * @brief ����д���ݵ�������(NRF24L01 FIFO)
  * @param reg : �Ĵ�����ַ
	* @param *pbuf : ���ݰ��׵�ַ
	* @param num : ���͵����ݸ���
  * @retval NRF24L01����״̬
  * @note  �ú�����Ҫ���ڽ�һ�����ݷŵ�NRF24L01�ķ���FIFO������
	* 	
  */
NRF_StatusTypedef NRF24L01_Read_Buf(uint8_t reg, uint8_t* pbuf, uint8_t num)
{
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t i = 0;
	
	NRF_CSN_SELECT();//��ȡSPI����
	timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
	status = SPI_RW(NRF_SPI,reg,&timeout);	//ѡ��Ҫд��ļĴ���
	if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
	{
		NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
		return NRF_TIMEOUT;	//���ش���
	}
	
	for(i = 0; i < num; i++)
	{
		timeout = NRF_TIMEOUT_VAL;//���÷��ʳ�ʱʱ��
		status = SPI_RW(NRF_SPI,0x00,&timeout);
		if(timeout == 0x00 && status == 0xff)//�ж��Ƿ�ʱ
		{
			NRF_CSN_DESELECT();	//�ͷ�SPI��ѡ
			return NRF_TIMEOUT;	//���ش���
		}
		*pbuf = status;//�������
		pbuf++;
	}
	
	NRF_CSN_DESELECT();//�ͷ�SPI����
	return NRF_SUCCESS;//���ز����ɹ�
}

/**
  * @brief ����д���ݵ�������(NRF24L01 FIFO)
  * @param reg : �Ĵ�����ַ
	* @param *pbuf : ���ݰ��׵�ַ
	* @param num : ���͵����ݸ���
  * @retval NRF24L01����״̬
  * @note  �ú�����Ҫ���ڽ�һ�����ݷŵ�NRF24L01�ķ���FIFO������
	* 	
  */
NRF_StatusTypedef A429_config(void)
{
	NRF_StatusTypedef status = NRF_ERROR;
	uint8_t test[5] = {0x06,0xBA,0xBA,0xBA,0x0C};
	
		delay_ms(10);
	//����������д�뷢�ͻ���
	//status = NRF24L01_Write_Buf(NRF_WRITE_REG|TX_ADDR, test, 1);WR_R1_CTL
	status = NRF24L01_Write_Buf(WR_R1_CTL, test, 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���
	
		status = NRF24L01_Write_Buf(WR_R1_PL1_MATCH, &test[1], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���
	
			status = NRF24L01_Write_Buf(WR_FLAG_ASSIGN, &test[4], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���
	
		return NRF_SUCCESS;	//NRF24L01ͨ������
}

NRF_StatusTypedef NRF24L01_Check(void)
{
	NRF_StatusTypedef status = NRF_ERROR;
	uint8_t result[20] = {0x00,0x00,0x00,0x00,0x00};
	uint8_t i = 0;
//	uint8_t display_staxx =0;
	
	
	//	status = NRF24L01_Write_Buf(MASTER_RST, test, 1);

	
	//�ӷ��ͻ����ȡ����
	//status = NRF24L01_Read_Buf(NRF_READ_REG|TX_ADDR, result, 1);
	status = NRF24L01_Read_Buf(RD_R1_CTL, result, 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���
	
	status = NRF24L01_Read_Buf(RD_R1_STA, &result[1], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���	

	status = NRF24L01_Read_Buf(RD_R1_FIFO, &result[2], 4);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���		

		status = NRF24L01_Read_Buf(RD_R1_PL1_CONTENT, &result[6], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���		
	

	
	
			status = NRF24L01_Read_Buf(RD_R1_PL1_MATCH, &result[9], 3);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ��
	
		status = NRF24L01_Read_Buf(RD_FLAG_ASSIGN, &result[12], 1);
	if(status != NRF_SUCCESS)return status;//NRF24L01ͨ�Ŵ���
	
			status = NRF24L01_Read_Buf(RD_FLAG_ASSIGN, &result[15], 1);
	if(status != NRF_SUCCESS)return status;
		if(0x30==result[8])
			return NRF_ERROR;
			
	/*
	//�ж϶�ȡ���������Ƿ�ͷ��͵�������ͬ
	for(i = 0; i < 1; i++)
	{
		if(result[i] != test[i])
			return NRF_ERROR;//���ݴ���
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
	
	
	
	return NRF_SUCCESS;	//NRF24L01ͨ������
	
}


/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/
