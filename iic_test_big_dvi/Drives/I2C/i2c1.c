
 
#include "i2c1.h"
//#include "i2c.h"

// I2C访问速度 = I2C_SPEED_1K / i2c_speed
//uint32_t	i2c_speed = I2C1_SPEED_1K/100;




/**
  * @brief  设置I2C速度
  * @param  speed : I2C速度，单位Kbps
  * @retval 返回设置前的I2C速度
	* @note   I2C速度设置范围是: 1Kbps ~ 400Kbps
  */
uint16_t I2C1_SetSpeed(uint16_t speed)
{
	uint16_t temp;
	
	//I2C速度必须小于400Kbps，大于 1Kbps
	if((speed > 400)|| (speed < 1)) return 0;
	
	temp = I2C1_SPEED_1K / i2c_speed;	//备份原来的i2c速度
	i2c_speed = I2C1_SPEED_1K / speed;	//设置新的i2c速度

	return temp;	//返回设置前的i2c速度
}
/**
  * @brief  模拟I2C接口初始化//i2c1
  * @param  None
  * @retval None
  * @note
	*		SCL1: 	PB6
	*		SDA1:	PB7
  */
void I2C1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//使能GPIOB外设时钟
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB, ENABLE );	
	
	//I2C的SDA和SCL都需要在硬件上连接上拉电阻，因此这里设置为上拉输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7);//根据以上配置初始化GPIO

	//设置SCL和SDA空闲状态为高电平
	I2C1_SCL = 1;
	I2C1_SDA = 1;
	
	I2C1_SetSpeed(100);//设置I2C访问速度为100Kbps
}


/**
	* @brief 产生I2C起始信号
  * @param  None
  * @retval None
  * @note
	*		请参考I2C通信协议，I2C起始信号：当SCL为高电平时，SDA由高变低
	*		如下图所示:方框部分表示I2C起始信号
	*           _____     |
	*        __|__   |    |  ___  ___  ___  ___  ___  ___  ___  ___  
	*   SDA:   |  \__|____|_/   \/   \/   \/   \/   \/   \/   \/   \     /
	*          |     |    | \___/\___/\___/\___/\___/\___/\___/\___/\___/
	*        __|_____|_   |   _    _    _    _    _    _    _    _    _
	*   SCL:   |     | \__|__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \_
	*          |_____|    | 
	*           start         D7   D6   D5   D4   D3   D2   D1   D0   ACK
	*/
void I2C1_Start(void)
{
	uint32_t i2c_delay = i2c_speed;
	
	SDA1_OUT();	//SDA设置为输出
	I2C1_SDA = 1;	//SDA: 高
	I2C1_SCL = 1;	//SCL: 高
	i2c_delay = i2c_speed;//延时>4.7us
	while(i2c_delay--){}
 	I2C1_SDA = 0;	//当SCL为高电平时，SDA由高变低
	i2c_delay = i2c_speed;//延时>4us
	while(i2c_delay--){}
	I2C1_SCL = 0;	//SCL变低，钳住I2C总线，准备发送或接收数据 
}

/**
	* @brief 产生I2C停止信号
  * @param  None
  * @retval None
  * @note
	*		请参考I2C通信协议，I2C停止信号：当SCL为高电平时，SDA由低变高
	*		发送完STOP信号后，SCL和SDA都为高电平，即释放了I2C总线
	*		如下图所示:方框部分表示I2C起始信号
	*                                    _____
	*         ___  ___  ___  ___        |   __|_
	*   SDA: /   \/   \/   \/   \       |  /  |
	*        \___/\___/\___/\___/\______|_/   |
	*         _    _    _    _    _    _|_____|_
	*   SCL: / \__/ \__/ \__/ \__/ \__/ |     |
	*                                   |_____|
	*        D3   D2   D1   D0   ACK     stop
	*/
void I2C1_Stop(void)
{
	uint32_t i2c_delay = i2c_speed;
	
	SDA1_OUT(); 		//SDA设置为输出
	I2C1_SDA = 0;	//SDA低电平
	I2C1_SCL = 1;	//SCL高电平
 	i2c_delay = i2c_speed;//延时>4us
	while(i2c_delay--){}
	I2C1_SDA = 1;	//STOP:当SCL为高电平时，SDA由低变高
	i2c_delay = i2c_speed;
	while(i2c_delay--){}//延时>4.7us						   	
}

/**
	* @brief  等待ACK应答信号
  * @param  None
  * @retval 1 - 未接收到应答信号ACK；0 - 接收到应答信号ACK
  * @note
	*		请参考I2C通信协议，检测ACK应答信号：当SCL为高电平时，读取SDA为低电平
	*		如下图所示:方框部分表示I2C起始信号
	*                             ________     _____
	*         ___  ___  ___  ___ | _      |   |   __|_
	*   SDA: /   \/   \/   \/   \|/ \     |   |  /  |
	*        \___/\___/\___/\___/|   \____|___|_/   |
	*         _    _    _    _   | _____  |  _|_____|
	*   SCL: / \__/ \__/ \__/ \__|/     \_|_/ |     |
	*                            |________|   |_____|
	*        D3   D2   D1   D0      ACK        stop
	*/
uint8_t I2C1_Wait_ACK(void)
{
	uint32_t i2c_delay = i2c_speed;
	uint8_t timeout = 0;
	
	SDA1_IN();			//SDA设置为输入
	I2C1_SDA = 1;	//SDA上拉输入
	I2C1_SCL=1;	//SCL设置为高电平
	i2c_delay = i2c_speed;
	while(i2c_delay--){}
	while(READ1_SDA == 1)//等待ACK
	{
		if(timeout++ > 250)
		{
			I2C1_Stop();
			return 1;
		}
	}
	I2C1_SCL = 0;//钳住I2C总线：时钟信号设为低电平
	return 0;  
}


/**
	* @brief  产生ACK应答信号
  * @param  None
  * @retval None
  * @note
	*		请参考I2C通信协议，产生ACK应答信号: 在SDA为低电平时，SCL产生一个正脉冲
	*		如下图所示:方框部分表示I2C起始信号
	*                             _____     _____
	*         ___  ___  ___  ___ |     |   |   __|_
	*   SDA: /   \/   \/   \/   \|     |   |  /  |
	*        \___/\___/\___/\___/|\____|___|_/   |
	*         _    _    _    _   |  _  |  _|_____|_
	*   SCL: / \__/ \__/ \__/ \__|_/ \_|_/ |     |
	*                            |_____|   |_____|
	*        D3   D2   D1   D0     ACK      stop
	*/
void I2C1_ACK(void)
{
	uint32_t i2c_delay = i2c_speed;
	
	I2C1_SCL = 0;	//低电平
	SDA1_OUT();		//设置SDA为输出
	I2C1_SDA = 0;	//ACK信号
	i2c_delay = i2c_speed;
	while(i2c_delay--){}//延时>4us
	I2C1_SCL = 1;	//高电平
	i2c_delay = i2c_speed;
	while(i2c_delay--){}//延时>4us
	I2C1_SCL = 0;	//钳住I2C总线：时钟信号设为低电平
}


/**
	* @brief  产生非应答信号NACK
  * @param  None
  * @retval None
  * @note
	*		请参考I2C通信协议，产生ACK应答信号: 在SDA为高电平时，SCL产生一个正脉冲
	*		如下图所示:方框部分表示I2C起始信号
	*                             _____      ______
	*         ___  ___  ___  ___ | ____|_   |    __|_
	*   SDA: /   \/   \/   \/   \|/    | \  |   /  |
	*        \___/\___/\___/\___/|     |  \_|__/   |
	*         _    _    _    _   |  _  |  __|______|_
	*   SCL: / \__/ \__/ \__/ \__|_/ \_|_/  |      |
	*                            |_____|    |______|
	*        D3   D2   D1   D0    NACK        stop
	*/	    
void I2C1_NACK(void)
{
	uint32_t i2c_delay = i2c_speed;
	
	I2C1_SCL = 0;	//低电平
	SDA1_OUT();		//SDA设置为输出
	I2C1_SDA = 1;	//NACK信号
	i2c_delay = i2c_speed;
	while(i2c_delay--){}//延时>4us
	I2C1_SCL = 1;	//高电平
	i2c_delay = i2c_speed;
	while(i2c_delay--){}//延时>4us
	I2C1_SCL = 0;	//钳住I2C总线：时钟信号设为低电平
}


/**
	* @brief  I2C发送一个字节
  * @param  None
  * @retval None
  * @note
	*		请参考I2C通信协议，产生ACK应答信号: 在SDA为高电平时，SCL产生一个正脉冲
	*		如下图所示:方框部分表示I2C起始信号
	*
	*           _____     |<------------I2C数据发送周期------------>|
	*        __|__   |    |  ___  ___  ___  ___  ___  ___  ___  ___ | _ 
	*   SDA:   |  \__|____|_/   \/   \/   \/   \/   \/   \/   \/   \|/ 
	*          |     |    | \___/\___/\___/\___/\___/\___/\___/\___/|\_
	*        __|_____|_   |   _    _    _    _    _    _    _    _  |  
	*   SCL:   |     | \__|__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \_|_
	*          |_____|    |                                         |
	*           start     |   D7   D6   D5   D4   D3   D2   D1   D0 |
	*/
void I2C1_Send_Byte(uint8_t data)
{                        
	uint8_t i = 0;
	uint32_t i2c_delay = i2c_speed;

	SDA1_OUT();									//SDA设为输出
	I2C1_SCL = 0;								//钳住I2C总线：SCL设为低电平
	for(i = 0; i < 8; i++)
	{
		if(data&0x80)I2C1_SDA = 1;	//高位先传
		else I2C1_SDA = 0;
		
		i2c_delay = i2c_speed;
		while(i2c_delay--){}			//延时>4us
	
		I2C1_SCL = 1;							//在SCL上产生一个正脉冲
		i2c_delay = i2c_speed;
		while(i2c_delay--){}			//延时>4us
			
		I2C1_SCL=0;
		i2c_delay = i2c_speed/3;
		while(i2c_delay--){}			//延时>1us
		data <<= 1;								//右移一位
	}
}


/**
	* @brief  从I2C读取一个字节
  * @param  ack : 0 - NACK; 1 - ACK
  * @retval 接收到的数据
  * @note
	*		请参考I2C通信协议，产生ACK应答信号: 在SDA为高电平时，SCL产生一个正脉冲
	*		如下图所示:方框部分表示I2C起始信号
	*
	*           _____     |<------------I2C数据读取周期(ACK)------------>|
	*        __|__   |    |  ___  ___  ___  ___  ___  ___  ___  ___      |
	*   SDA:   |  \__|____|_/   \/   \/   \/   \/   \/   \/   \/   \     |
	*          |     |    | \___/\___/\___/\___/\___/\___/\___/\___/\____|_
	*        __|_____|_   |   _    _    _    _    _    _    _    _    _  |
	*   SCL:   |     | \__|__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \_|_
	*          |_____|    |                                              |
	*           start     |   D7   D6   D5   D4   D3   D2   D1   D0   ACK
	*
	*           _____     |<------------I2C数据读取周期(NACK)----------->|
	*        __|__   |    |  ___  ___  ___  ___  ___  ___  ___  ___  ____|_
	*   SDA:   |  \__|____|_/   \/   \/   \/   \/   \/   \/   \/   \/    |
	*          |     |    | \___/\___/\___/\___/\___/\___/\___/\___/     |
	*        __|_____|_   |   _    _    _    _    _    _    _    _    _  |
	*   SCL:   |     | \__|__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \__/ \_|_
	*          |_____|    |                                              |
	*           start     |   D7   D6   D5   D4   D3   D2   D1   D0  NACK
	*/
uint8_t I2C1_Read_Byte(uint8_t ack)
{
	uint8_t i, receive = 0x00;
	uint32_t i2c_delay = i2c_speed;
	
	I2C1_SCL = 0;									//SCL低电平
	SDA1_IN();											//SDA设置为输入
	for(i = 0; i < 8; i++)
	{
		i2c_delay = i2c_speed;
		while(i2c_delay--);
		I2C1_SCL = 1;								//高电平
		i2c_delay = i2c_speed;
		while(i2c_delay--);
		receive <<= 1;
		if(READ1_SDA) receive |= 1;	//高位在前
		I2C1_SCL = 0;
  }
	if (ack == 0) I2C1_NACK();			//发送NACK
	else I2C1_ACK();								//发送ACK
	
	return receive;								//返回接收到的数据
}



/* ---------------------------以下部分是封装好的I2C读写函数--------------------------- */

//具体到某一个器件，请仔细阅读器件规格书关于I2C部分的说明，因为某些器件I2C的读写操作会
//有一些差异，下面的代码我们在绝大多数的I2C器件中，都是验证OK的！

/**
  * @brief  向设备指定地址写入单一Byte数据
  * @param  DevAddr : I2C从设备地址
  * @param  DataAddr: 需要访问的设备内地址(如寄存器地址，EEPROM地址等)
  * @param  Data    : 写入的数据
  * @retval I2C访问的结果: I2C_SUCCESS / I2C_TIMEOUT / I2C_ERROR
	* @note   
	*   1 - 设备地址DevAddr高7bit是固定的，最低为是读/写(R/W)位，1为读，0为写
	*		2 - 时序：
	*           _______________________________________
	*          | |         |   |        |   |    |   | |
	*   Master:|S|DevAddr+W|   |DataAddr|   |Data|   |P|
	*          |_|_________|___|________|___|____|___|_|
	*           _______________________________________
	*          | |         |   |        |   |    |   | |
	*   Slave: | |         |ACK|        |ACK|    |ACK| |
	*          |_|_________|___|________|___|____|___|_|
  */
I2C_StatusTypeDef I2C1_WriteOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t Data)
{
	I2C1_Start();													//Master发送起始信号
	I2C1_Send_Byte(DevAddr);								//Master发送从设备地址
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	I2C1_Send_Byte(DataAddr);							//发送数据地址
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	I2C1_Send_Byte(Data);									//发送数据
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	I2C1_Stop();														//发送停止信号
	return I2C_SUCCESS;
}



/**
  * @brief  从指定设备读取1Byte数据
  * @param  DevAddr : I2C从设备地址
  * @param  DataAddr: 需要访问的设备内地址(如寄存器地址，EEPROM地址等)
  * @param  *Data   : 数据的存放地址
  * @retval I2C访问的结果: I2C_SUCCESS / I2C_TIMEOUT / I2C_ERROR
	* @note   
	*   1 - 设备地址DevAddr高7bit是固定的，最低为是读/写(R/W)位，1为读，0为写
	*		2 - 时序：
	*           _________________________________________________________
	*          | |         |   |        |    | |         |   |    |    | |
	*   Master:|S|DevAddr+W|   |DataAddr|    |S|DevAddr+R|   |    |NACK|P|
	*          |_|_________|___|________|____|_|_________|___|____|____|_|
	*           _________________________________________________________
	*          | |         |   |        |    | |         |   |    |    | |
	*   Slave: | |         |ACK|        |ACK | |         |ACK|Data|    | |
	*          |_|_________|___|________|____|_|_________|___|____|____|_|
  */
I2C_StatusTypeDef I2C1_ReadOneByte(uint8_t DevAddr, uint8_t DataAddr, uint8_t* Data)
{
	I2C1_Start();													//Master发送起始信号
	I2C1_Send_Byte(DevAddr);								//Master发送从设备地址
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	I2C1_Send_Byte(DataAddr);							//发送数据地址
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	
	I2C1_Start();													//Master发送起始信号
	I2C1_Send_Byte(DevAddr+1);							//Master发送从设备读地址
	if(I2C1_Wait_ACK()) return I2C_TIMEOUT;//等待ACK超时错误
	*Data = I2C1_Read_Byte(0);							//读数据，NACK
	I2C1_Stop();														//发送停止信号
	return I2C_SUCCESS;
}




