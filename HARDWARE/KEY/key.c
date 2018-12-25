#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"


void KEY_Init(void)
{
	
	GPIO_InitTypeDef GPIO_str;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	
	GPIO_str.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
	GPIO_str.GPIO_Mode=GPIO_Mode_IN;
	GPIO_str.GPIO_Speed=GPIO_High_Speed;
	GPIO_str.GPIO_PuPd=GPIO_PuPd_UP;
	
	GPIO_Init(GPIOF,&GPIO_str);
	
	
}

//mode1:连续    mode0:不连续
//KEY0:1 KEY1:2 KEY2:3 无按键:0
int key_scan(int mode)
{
	static int status=1;
	
	if(mode)status=1;
	if((KEY0==0||KEY1==0||KEY2==0)&&status)
	{
		delay_ms(10);
		status=0;
		if(KEY0==0)
		{
			printf("KEY 0 is preesed.\r\n");
			return 1;
		}
		else if(KEY1==0)
		{
			printf("KEY 1 is preesed.\r\n");
			return 2;
		}
		else if(KEY2==0)
		{
			printf("KEY 2 is preesed.\r\n");
			return 3;
		}
	}
	else if(KEY1==1&&KEY2==1&&KEY0==1)
	{
		status=1;
		return 0;
	}
	
	return 0;
}
