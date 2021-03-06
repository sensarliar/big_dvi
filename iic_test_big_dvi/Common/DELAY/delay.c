/*******************************************************************************
 * @name    : 延时函数文件
 * @author  : 布谷鸟
 * @web     : WWW.UCORTEX.COM
 * @version : V1.0
 * @date    : 2014-04-03
 * @MDK     : KEIL MDK4.72a & KEL MDK 5.10
 * @brief   : 毫秒和微秒的延时，供后面的实验例程调用
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
 *
 ******************************************************************************/
#include "delay.h"

/**
  * @brief  设置SysTick计数时钟为HCLK/8
  * @param  None
  * @retval None
	* @note   
	*    当HCLK为72MHz时，最长延迟时间为0xFFFFFF/9000000 (S) = 1864ms
  */
void Delay_Init(void)
{
	/* 设置SysTick的计数时钟频率为HCLK/8，如果HCLK是72MHz，则SysTick计数时钟是9MHz*/
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
}

/**
  * @brief  ms延时函数
	* @param  xms: 延时的毫秒数
  * @retval None
	* @note   
	*    通过下面的公式来改变SysTick的中断时间:
  *                          
  *    重载值(Reload Value) = SysTick计数时钟频率 (Hz) x  希望中断的时间间隔 (s)
  *  
  *    - 重载值作为SysTick_Config()的参数传递
	*    - 重载值不允许超过 0xFFFFFF，因此最长可以延时1864ms
  */
void delay_ms(uint16_t xms)
{
	uint32_t reload;
	
	reload = SystemCoreClock/8000;//延时1ms的重载值
	reload *= xms;//延时xms的重载值。
	SysTick->LOAD = (reload & 0xFFFFFF) - 1;//加载SysTick重载值
	SysTick->VAL = 0;//计数值清零
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//开始倒数计数
	while(!((SysTick->CTRL) & (1 << 16)));//等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭滴答计数器
	SysTick->VAL = 0x00;//清空计数器
}

/**
  * @brief  us延时函数
	* @param  xus: 延时的微秒数
  * @retval None
	* @note   
	*		通过下面的公式来改变SysTick的中断时间:
  *                          
  *   重载值(Reload Value) = SysTick计数时钟频率 (Hz) x  希望中断的时间间隔 (s)
  *  
  *   - 重载值作为SysTick_Config()的参数传递
	*		- 重载值不允许超过 0xFFFFFF，因此最长可以延时1864ms
  */
void delay_us(uint32_t xus)
{
	uint32_t reload;
	
	reload = SystemCoreClock/8000000;//延时1us的重载值
	reload *= xus;//延时xus的重载值。
	SysTick->LOAD = (reload & 0xFFFFFF) - 1;//加载SysTick重载值
	SysTick->VAL = 0;//计数值清零
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//开始倒数计数
	while(!((SysTick->CTRL) & (1 << 16)));//等待时间到达
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;//关闭滴答计数器
	SysTick->VAL = 0x00;//清空计数器
}

/********************* (C) COPYRIGHT 2014 WWW.UCORTEX.COM **********END OF FILE**********/
