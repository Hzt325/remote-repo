#include "stm32f10x.h"                  // Device header
#include "Delay.h"



void Xunji_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
}

uint8_t Read_Xunji(uint16_t GPIO_Pin)
{
		uint8_t temp;
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin) == 0)	temp = 1;
		else temp = 0;//1ºÚÏß
	
		return temp;
}