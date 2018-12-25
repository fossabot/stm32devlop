#include "usart.h"
#include "delay.h"
#include "led.h"
#include "sys.h"
#include "beep.h"
#include "key.h"
#include "lcd.h"
#include "remote.h"
#include "exti.h"

void _show_info(void);

extern double frequency,duty;

int main(void)
{
	int key_status,remote_status;
	delay_init(168);
	LED_Init();
	KEY_Init();
 	Remote_Init();	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	uart_init(115200);		
 	LCD_Init();   
	EXTIX_Init();
	POINT_COLOR=BLUE;  
	PWMMODE_84psc(frequency,duty); 	
	_show_info();
	while(1)
	{
		remote_status=Remote_Scan();
		if(remote_status)
		{
			if(remote_status==34)
			{
				duty+=5.0;
				if(duty>99)duty=99.9;
				PWMMODE_84psc(frequency,duty);
				_show_info();
				delay_ms(1000);
			}
			else if(remote_status==194)
			{
				duty-=5.0;
				if(duty>99)duty=99.9;
				PWMMODE_84psc(frequency,duty);
				_show_info();
				delay_ms(1000);
			}
			else if(remote_status==98)
			{
				frequency+=100.0;
				if(frequency>4999)frequency=5000.0;
				PWMMODE_84psc(frequency,duty);
				_show_info();
				delay_ms(1000);
			}
			else if(remote_status==168)
			{
				frequency-=100.0;
				if(frequency<499)frequency=500.0;
				PWMMODE_84psc(frequency,duty);
				_show_info();
				delay_ms(1000);
			}
			else if(remote_status==162)
			{
				_input_frequency();
			}
			else if(remote_status==226)
			{
				_input_duty();
			}
			else
			{
				delay_ms(50);
			}
			remote_status=Remote_Scan();
			remote_status=Remote_Scan();
		}
	}
}

void _show_info(void)
{
	LCD_Clear(WHITE);
	char prin1[100];
	char prin2[100];
	sprintf(prin1,"Frequency:%.2f KHz",frequency/1000);
	sprintf(prin2,"Duty:%.2f%%",duty);
	printf("Frequency:%.2f KHz  Duty:%.2f%% \r\n",frequency/1000,duty);
	LCD_ShowString(30,70,200,16,16,prin1);
 	LCD_ShowString(30,110,200,16,16,prin2);
	return;
}

