/*******************************************************************************
 * @name    : I2Cͨ��ʵ��
 * @author  : ������
 * @web     : WWW.UCORTEX.COM
 * @version : V1.2
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : ʵ����ͨ��STM32ͨ��IO��ģ��I2Cͨ�ŵĵײ㺯��
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
 * ���̽��ܣ�
 * 1.1 ϵͳ�ϵ�󣬳�ʼ��I2C�˿�PB10(SCL)��PB11(SDA)��
 * 1.2 JoyStick����KEY_UPʵ�ֲ���������(��ߵ�400Kbps)��KEY_DOWNʵ�ֲ����ʵļ��٣�
 *     KEY_CENTERʵ�ֶ�I2C���豸MPU6050��һ�ζ�д���ʡ�
 * 1.3 OLED��ʾ��ǰI2C���ٶ�(Ĭ����100Kbps)��I2C��ȡ������ֵ���Լ�I2C�����Ƿ�ɹ���
 *-----------------------------------------------------------------------------
 * @history
 * ----------------------------------------------------------------------------
 * ����ʱ�䣺2014-04-06    �����ˣ�������
 * �汾��¼��V1.0
 * �������ݣ��½�
 * ----------------------------------------------------------------------------
 *
 ******************************************************************************/
#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "key.h"
#include "usart.h"
#include "oled.h"
#include "exti.h"
#include "adc.h"
#include "joystick.h"
#include "timer.h"
#include "rtc.h"
#include "standby.h"
#include "dma.h"
#include "beep.h"
#include "stm32_eeprom.h"
#include "spi.h"
#include "nrf24l01.h"
//#include "i2c.h"
#include "mpu6050.h"
#include "i2c1.h"

#define DVI_REG_ADDR 0x00
//#define DVI_DevAddr 0x08
//#define DVI_Dev2Addr 0x00
#define DPU1_DVI1_ADDR 0x00<<1
#define DPU1_DVI2_ADDR 0x01<<1
#define DPU1_DVI3_ADDR 0x02<<1
#define DPU2_DVI1_ADDR 0x03<<1
#define DPU2_DVI2_ADDR 0x04<<1
#define DPU2_DVI3_ADDR 0x05<<1
//DPU1_DVI1_ADDR   ----DPU2_DVI1_ADDR
//I2C2:
#define DPU1_DVI4_ADDR 0x00<<1
#define DPU1_DVI5_ADDR 0x01<<1
#define EVS_DVI_ADDR 0x02<<1
#define DPU2_DVI4_ADDR 0x03<<1
#define DPU2_DVI5_ADDR 0x04<<1
//32~38W
//7-bit slave address (0000ADD[2:0]) followed by an 8th bit which is the data direction bit (R/W). A zero
//indicates a WRITE and a 1 indicates a READ.

///BUFF FOR dpu1:dvi1,dvi2,dvi3
void modify_buff(uint8_t* buffer)
{
	buffer[0] = 0x0;
	//buffer[1] = 0x80;
	buffer[1] = 0x80;//8f
		//A
//	buffer[2] = 0x0C;
//	buffer[2] = 0x04;
	buffer[2] = 0x0C;
//	buffer[3] = 0x0D;
	buffer[3] = 0x0F;
//	buffer[4] = 0x00;
	buffer[4] = 0x13;
//	buffer[4] = 0x11;
	buffer[5] = 0x00;
	//B
	buffer[6] = 0x0C;
	buffer[7] = 0x0F;
	buffer[8] = 0x13;
	buffer[9] = 0x00;
	//C
	//buffer[10] = 0xF5;
	buffer[10] = 0x0C;
	buffer[11] = 0x0F;
	buffer[12] = 0x13;
	buffer[13] = 0x00;

	buffer[14] = 0x00;
	buffer[15] = 0x11;
	buffer[16] = 0x00;
	buffer[17] = 0x00;
	
	buffer[18] = 0x0F;
}


///BUFF FOR dpu2:dvi1,dvi2,dvi3
void modify_buff2(uint8_t* buffer)
{
	buffer[0] = 0x0;
	buffer[1] = 0x80;//8f
		//A
	buffer[2] = 0x0C;
	buffer[3] = 0x80;
	buffer[4] = 0x13;//33
	buffer[5] = 0x00;
	//B
	buffer[6] = 0x0C;
	buffer[7] = 0x80;
	buffer[8] = 0x13;
	buffer[9] = 0x00;
	//C
	//buffer[10] = 0xF5;
	buffer[10] = 0xF5;
	buffer[11] = 0x0F;
	buffer[12] = 0x10;
	buffer[13] = 0x00;

	buffer[14] = 0x00;
	buffer[15] = 0x11;
	buffer[16] = 0x00;
	buffer[17] = 0x00;
	
	buffer[18] = 0x0F;
}




/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	uint32_t t=0;
	uint8_t data = 0;
	uint8_t key = 0;
	//uint16_t speed = 100;
	uint8_t i = 0;
	uint8_t flag =0;
	
	uint8_t buffer[19];

	
/*
	LED_Init();				//LED IO��ʼ��
	Delay_Init();			//��ʱ��ʼ��
	COM_Init(COM1, 115200);//����1��ʼ��
	OLED_Init();			//��ʼ��OLED
	ADC_Initialize();	//ADC��ʼ��
	JoyStick_Init();	//ҡ�˰���(JoyStick)��ʼ��
	BEEP_Init();			//��������ʼ��
  EE_Init();				//FLASH ģ��EEPROM��ʼ��
	I2C1_Init();				//��ʼ��I2C�ӿ�
	
	//��ʾ��ʾ��Ϣ
	OLED_ShowString(0,0,"WWW.UCORTEX.COM");	
	OLED_ShowString(0,16,"SPEED: 100 Kbps");
	OLED_ShowString(0,32,"DATA = 0x  ");
	OLED_ShowString(0,48,"RESULT: ");
	OLED_Refresh_Gram();
	*/
	
		Delay_Init();			//��ʱ��ʼ��
			I2C1_Init();				//��ʼ��I2C�ӿ�
			delay_ms(500);
			I2C_Init();				//��ʼ��I2C�ӿ�
			delay_ms(500);

////////DVI 

	for(i = 0; i < 19; i++)
	{
			I2C1_ReadOneByte(DPU1_DVI1_ADDR, DVI_REG_ADDR+i, &buffer[i]);
	}
	//		I2C_ReadOneByte(DVI_DevAddr, DVI_REG_ADDR+0x12, &buffer[11]);
		//	I2C_ReadOneByte(DVI_DevAddr, DVI_REG_ADDR+0x6, &buffer[12]);
			modify_buff(buffer);
	////////DVI  UP BAN
	for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU1_DVI1_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
		for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU1_DVI2_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
		for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU1_DVI3_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
	
		////////DVI  DOWN BAN
	
			for(i = 0; i < 19; i++)
	{
			I2C_WriteOneByte(DPU1_DVI4_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
		for(i = 0; i < 19; i++)
	{
			I2C_WriteOneByte(DPU1_DVI5_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
	
////// POWER OFF B_PORT IN
//	buffer[7] = 0x80;
	
	//nihao
		for(i = 0; i < 19; i++)
	{
			I2C_WriteOneByte(EVS_DVI_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
				for(i = 0; i < 19; i++)
	{
			I2C_WriteOneByte(DPU2_DVI4_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
		for(i = 0; i < 19; i++)
	{
			I2C_WriteOneByte(DPU2_DVI5_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
	
	
		modify_buff2(buffer);

		for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU2_DVI1_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
	//delay_ms(500);
		for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU2_DVI2_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
		for(i = 0; i < 19; i++)
	{
			I2C1_WriteOneByte(DPU2_DVI3_ADDR, DVI_REG_ADDR+i, buffer[i]);
	}
	if(1)
	{
		buffer[1] = 0x8F;
		
			I2C1_WriteOneByte(DPU1_DVI1_ADDR, DVI_REG_ADDR+1, buffer[1]);
	
			I2C1_WriteOneByte(DPU1_DVI2_ADDR, DVI_REG_ADDR+1, buffer[1]);
	
			I2C1_WriteOneByte(DPU1_DVI3_ADDR, DVI_REG_ADDR+1, buffer[1]);
		

			I2C_WriteOneByte(DPU1_DVI4_ADDR, DVI_REG_ADDR+1, buffer[1]);
	
			I2C_WriteOneByte(DPU1_DVI5_ADDR, DVI_REG_ADDR+1, buffer[1]);
	

	}
	
 		while(1){}
			
			
	while(0)
	{
		key = JoyStick_Scan(0);
		if(key == KEY_CENTER)//����һ��I2C��д
		{

	//		I2C_ReadBurst(DVI_DevAddr, DVI_REG_ADDR+0x2, buffer, 5);
//			data = MPU6050_getDeviceID();

	if(flag == 1)
	{
		buffer[1] = 0x80;
		flag = 0;
	}
	else
	{
		buffer[1] = 0x8F;
		flag = 1;
	}
		
	I2C1_WriteOneByte(DPU1_DVI1_ADDR, DVI_REG_ADDR+0x1, buffer[1]);
	
	/*
			I2C_WriteBurst(DVI_DevAddr, DVI_REG_ADDR, buffer, 5);
			I2C_WriteBurst(DVI_DevAddr, DVI_REG_ADDR+0x06, buffer, 3);
			I2C_WriteBurst(DVI_DevAddr, DVI_REG_ADDR+0x0A, buffer, 3);
			I2C_WriteBurst(DVI_DevAddr, DVI_REG_ADDR+0x10, buffer, 3);
			*/
			OLED_ShowxNum(72,32,buffer[3],2,16,0x13);
		//	OLED_ShowxNum(72,32,data,2,16,0x13);
			if(data == 0x68)
				OLED_ShowString(64,48,"SUCCESS");
			else
				OLED_ShowString(64,48,"ERROR  ");
		}

		
		OLED_Refresh_Gram();
		delay_ms(50);
		if(t++>5)
		{
			t=0;
			LED=!LED;//����״ָ̬ʾ
		}
	}
}


/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/