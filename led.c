/**
 ****************************************************************************************************
 * @author      创想电子
 ****************************************************************************************************
 * @attention
 * 主控系统:STM32F103C8T6最小系统板
 * 联系方式(VX):l1413930547
 ****************************************************************************************************
 */
#include "led.h"
#include "delay.h"

void LED_Init(void)
{
	//开启GPIOB时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);

	//配置LED引脚为模拟输出模式
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_Init(GPIOA , &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC , &GPIO_InitStructure);
	
	LED = 0;
	JDQ = 0;
}




