#include "beep.h"


void BEEP_Init(void)
{
	
	GPIO_InitTypeDef GPIO_str;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF,ENABLE);
	
	GPIO_str.GPIO_Pin=GPIO_Pin_8;
	GPIO_str.GPIO_Mode=GPIO_Mode_OUT;
	GPIO_str.GPIO_Speed=GPIO_Fast_Speed;
	GPIO_str.GPIO_OType=GPIO_OType_PP;
	GPIO_str.GPIO_PuPd=GPIO_PuPd_DOWN;
	
	GPIO_Init(GPIOF,&GPIO_str);
	
	GPIO_ResetBits(GPIOF,GPIO_Pin_8);
	
}

