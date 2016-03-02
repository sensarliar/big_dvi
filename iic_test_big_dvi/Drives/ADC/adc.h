/*******************************************************************************
 * @name    : ADC�ײ�����ͷ�ļ�
 * @author  : ������
 * @web     : WWW.UCORTEX.COM
 * @version : V1.2
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : ����PA0Ϊģ������˿ڣ�ʹ���ڲ�STM32�ڲ��¶ȴ�����
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
 * PA0 -> ADC1_CH0
 * TEMP-> ADC1_CH16
 *
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2014-04-06    �����ˣ�������
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2014-04-06    �����ˣ�������
 * �汾��¼��V1.1
 * �������ݣ���ADC_Initialize�������˿���STM32�ڲ��¶ȴ�������
 * ADC_TempSensorVrefintCmd(ENABLE); //�����ڲ��¶ȴ�����������Ҫ����ʱ��ע�͵�
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2014-04-07    �����ˣ�������
 * �汾��¼��V1.2
 * �������ݣ�������ADC DMA������ʽ��void ADC1_DMAInitialize(void)
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/

#ifndef __ADC_H
#define __ADC_H
#include "stm32f10x.h"

//�Ƿ�ʹ��ADC DMA����
#define 	ADC_DMA_TRANSFER		1 //�������Ҫ֧��DMA���붨��Ϊ0

#if ADC_DMA_TRANSFER
extern __IO uint16_t ADC_Buffer[2];
#define ADC1_DR_Address    ((uint32_t)0x4001244C)//ADC1���ݼĴ���
#endif

#define ADC_TEMP_CH  	ADC_Channel_16 //�¶ȴ�����ͨ��

void ADC_Initialize(void);
uint16_t Get_Adc(ADC_TypeDef* ADCx, uint8_t ch);
uint16_t Get_Adc_Average(ADC_TypeDef* ADCx, uint8_t ch);
#if ADC_DMA_TRANSFER
void ADC1_DMAInitialize(void);
#endif

#endif

/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/