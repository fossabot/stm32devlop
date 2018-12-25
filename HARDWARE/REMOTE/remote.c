#include "remote.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//����ң�ؽ������� ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved								  
//////////////////////////////////////////////////////////////////////////////////


//����ң�س�ʼ��
//����IO�Լ�TIM2_CH1�����벶��
double frequency=3000.0,duty=50.0;

void Remote_Init(void)    			  
{		
  GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_ICInitTypeDef  TIM1_ICInitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);//TIM1ʱ��ʹ�� 

  //GPIOA8  ���ù���,����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1); //GPIOA8����ΪTIM1
	
	TIM_TimeBaseStructure.TIM_Prescaler=167;  ////Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=10000;   //�趨�������Զ���װֵ ���10ms���  
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure); 
  	  
 	//��ʼ��TIM2���벶�����
	TIM1_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
  TIM1_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
  TIM1_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //ӳ�䵽TI1��
  TIM1_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
  TIM1_ICInitStructure.TIM_ICFilter = 0x03;//IC1F=0003 8����ʱ��ʱ�������˲�
  TIM_ICInit(TIM1, &TIM1_ICInitStructure);//��ʼ����ʱ��2���벶��ͨ��
	 
 
	
	TIM_ITConfig(TIM1,TIM_IT_Update|TIM_IT_CC1,ENABLE);//��������ж� ,����CC1IE�����ж�	
  TIM_Cmd(TIM1,ENABLE ); 	 	//ʹ�ܶ�ʱ��1
 
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//��ʼ��NVIC�Ĵ���

  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_TIM10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//��ʼ��NVIC�Ĵ���
}

//ң��������״̬
//[7]:�յ����������־
//[6]:�õ���һ��������������Ϣ
//[5]:����	
//[4]:����������Ƿ��Ѿ�������								   
//[3:0]:�����ʱ��
u8 	RmtSta=0;	  	  
u16 Dval;		//�½���ʱ��������ֵ
u32 RmtRec=0;	//������յ�������	   		    
u8  RmtCnt=0;	//�������µĴ���	 
//��ʱ��1����ж�
void TIM1_UP_TIM10_IRQHandler(void)
{
 
  if(TIM_GetITStatus(TIM1,TIM_IT_Update)==SET) //����ж�
	{
		if(RmtSta&0x80)//�ϴ������ݱ����յ���
		{	
			RmtSta&=~0X10;						//ȡ���������Ѿ���������
			if((RmtSta&0X0F)==0X00)RmtSta|=1<<6;//����Ѿ����һ�ΰ����ļ�ֵ��Ϣ�ɼ�
			if((RmtSta&0X0F)<14)RmtSta++;
			else
			{
				RmtSta&=~(1<<7);//���������ʶ
				RmtSta&=0XF0;	//��ռ�����	
			}						 	   	
		}							    
	}
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);  //����жϱ�־λ 
} 
//��ʱ��1���벶���жϷ������	 
void TIM1_CC_IRQHandler(void)
{ 		    	 
	if(TIM_GetITStatus(TIM1,TIM_IT_CC1)==SET) //������(CC1IE)�ж�
	{	  
		if(RDATA)//�����ز���
		{
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Falling);		//CC1P=1 ����Ϊ�½��ز���
			TIM_SetCounter(TIM1,0);	   	//��ն�ʱ��ֵ
			RmtSta|=0X10;					//����������Ѿ�������
		}else //�½��ز���
		{
			Dval=TIM_GetCapture1(TIM1);//��ȡCCR1Ҳ������CC1IF��־λ
			TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Rising); //CC1P=0	����Ϊ�����ز���
			if(RmtSta&0X10)					//���һ�θߵ�ƽ���� 
			{
 				if(RmtSta&0X80)//���յ���������
				{
					
					if(Dval>300&&Dval<800)			//560Ϊ��׼ֵ,560us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=0;	//���յ�0	   
					}else if(Dval>1400&&Dval<1800)	//1680Ϊ��׼ֵ,1680us
					{
						RmtRec<<=1;	//����һλ.
						RmtRec|=1;	//���յ�1
					}else if(Dval>2200&&Dval<2600)	//�õ�������ֵ���ӵ���Ϣ 2500Ϊ��׼ֵ2.5ms
					{
						RmtCnt++; 		//������������1��
						RmtSta&=0XF0;	//��ռ�ʱ��		
					}
 				}else if(Dval>4200&&Dval<4700)		//4500Ϊ��׼ֵ4.5ms
				{
					RmtSta|=1<<7;	//��ǳɹ����յ���������
					RmtCnt=0;		//�����������������
				}						 
			}
			RmtSta&=~(1<<4);
		}				 		     	    					   
	}
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC1);  //����жϱ�־λ 
}
//����������
//����ֵ:
//	 0,û���κΰ�������
//����,���µİ�����ֵ.
u8 Remote_Scan(void)
{        
	u8 sta=0;       
    u8 t1,t2;  
	if(RmtSta&(1<<6))//�õ�һ��������������Ϣ��
	{ 
	    t1=RmtRec>>24;			//�õ���ַ��
	    t2=(RmtRec>>16)&0xff;	//�õ���ַ���� 
 	    if((t1==(u8)~t2)&&t1==REMOTE_ID)//����ң��ʶ����(ID)����ַ 
	    { 
	        t1=RmtRec>>8;
	        t2=RmtRec; 	
	        if(t1==(u8)~t2)sta=t1;//��ֵ��ȷ	 
		}   
		if((sta==0)||((RmtSta&0X80)==0))//�������ݴ���/ң���Ѿ�û�а�����
		{
		 	RmtSta&=~(1<<6);//������յ���Ч������ʶ
			RmtCnt=0;		//�����������������
		}
	}  
    return sta;
}

void _input_frequency(void)
{
	char prin1[20]={0};
	_Bool status=1;
	int current=0;
	double new_frequency=0.0;
	double position=1.0;
	int remote_status;
	LCD_ShowString(30,150,220,16,16,"Please enter new frequency!");
	while(1)
	{
		remote_status=Remote_Scan();
		if(remote_status)
		{
			if(remote_status==104)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=1;
				}
				else
				{
					position/=10.0;
					new_frequency+=1*position;
				}
				prin1[current]='1';
				current++;
			}
			else if(remote_status==152)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=2;
				}
				else
				{
					position/=10.0;
					new_frequency+=2*position;
				}
				prin1[current]='2';
				current++;
			}
			else if(remote_status==176)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=3;
				}
				else
				{
					position/=10.0;
					new_frequency+=3*position;
				}
				prin1[current]='3';
				current++;
			}
			else if(remote_status==48)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=4;
				}
				else
				{
					position/=10.0;
					new_frequency+=4*position;
				}
				prin1[current]='4';
				current++;
			}
			else if(remote_status==24)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=5;
				}
				else
				{
					position/=10.0;
					new_frequency+=5*position;
				}
				prin1[current]='5';
				current++;
			}
			else if(remote_status==122)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=6;
				}
				else
				{
					position/=10.0;
					new_frequency+=6*position;
				}
				prin1[current]='6';
				current++;
			}
			else if(remote_status==16)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=7;
				}
				else
				{
					position/=10.0;
					new_frequency+=7*position;
				}
				prin1[current]='7';
				current++;
			}
			else if(remote_status==56)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=8;
				}
				else
				{
					position/=10.0;
					new_frequency+=8*position;
				}
				prin1[current]='8';
				current++;
			}
			else if(remote_status==90)
			{
				if(status)
				{
					new_frequency*=10;
					new_frequency+=9;
				}
				else
				{
					position/=10.0;
					new_frequency+=9*position;
				}
				prin1[current]='9';
				current++;
			}
			else if(remote_status==66)
			{
				if(status)
				{
					new_frequency*=10;
				}
				else
				{
					position/=10.0;
				}
				prin1[current]='0';
				current++;
			}
			else if(remote_status==224)
			{
				status=0;
				prin1[current]='.';
				current++;
			}
			else if(remote_status==2)
			{
				if(new_frequency<499 || new_frequency >5001)
				{
					LCD_ShowString(30,230,200,16,16,"Input Error!");
					delay_ms(2000);
					_show_info();
					break;
				}
				else
				{
					frequency=new_frequency;
					PWMMODE_84psc(frequency,duty);
					_show_info();
					break;
				}
			}
			else if(remote_status==82)
			{
				if(current==0)
				{
					continue;
				}
				if(status)
				{
					new_frequency=(int)(new_frequency/10);
					current--;
					prin1[current]=0;
				}
				else
				{
					if(position>0.5)
					{
						position=1.0;
						status=1;
						current--;
						prin1[current]=0;
					}
					else
					{
						current--;
						new_frequency-=(prin1[current]-'0'+1)*position;
						position*=10;
						prin1[current]=0;
					}
				}
				_show_info();
				LCD_ShowString(30,150,220,16,16,"Please enter new frequency!");
			}
			LCD_ShowString(30,190,200,16,16,prin1);
			remote_status=Remote_Scan();
			delay_ms(1000);
			remote_status=Remote_Scan();
		}
		else
		{
			delay_ms(50);
		}
	}
}

void _input_duty(void)
{
	char prin1[20]={0};
	_Bool status=1;
	int current=0;
	double new_duty=0.0;
	double position=1.0;
	int remote_status;
	LCD_ShowString(30,150,220,16,16,"Please enter new duty!");
	while(1)
	{
		remote_status=Remote_Scan();
		if(remote_status)
		{
			if(remote_status==104)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=1;
				}
				else
				{
					position/=10.0;
					new_duty+=1*position;
				}
				prin1[current]='1';
				current++;
			}
			else if(remote_status==152)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=2;
				}
				else
				{
					position/=10.0;
					new_duty+=2*position;
				}
				prin1[current]='2';
				current++;
			}
			else if(remote_status==176)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=3;
				}
				else
				{
					position/=10.0;
					new_duty+=3*position;
				}
				prin1[current]='3';
				current++;
			}
			else if(remote_status==48)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=4;
				}
				else
				{
					position/=10.0;
					new_duty+=4*position;
				}
				prin1[current]='4';
				current++;
			}
			else if(remote_status==24)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=5;
				}
				else
				{
					position/=10.0;
					new_duty+=5*position;
				}
				prin1[current]='5';
				current++;
			}
			else if(remote_status==122)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=6;
				}
				else
				{
					position/=10.0;
					new_duty+=6*position;
				}
				prin1[current]='6';
				current++;
			}
			else if(remote_status==16)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=7;
				}
				else
				{
					position/=10.0;
					new_duty+=7*position;
				}
				prin1[current]='7';
				current++;
			}
			else if(remote_status==56)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=8;
				}
				else
				{
					position/=10.0;
					new_duty+=8*position;
				}
				prin1[current]='8';
				current++;
			}
			else if(remote_status==90)
			{
				if(status)
				{
					new_duty*=10;
					new_duty+=9;
				}
				else
				{
					position/=10.0;
					new_duty+=9*position;
				}
				prin1[current]='9';
				current++;
			}
			else if(remote_status==66)
			{
				if(status)
				{
					new_duty*=10;
				}
				else
				{
					position/=10.0;
				}
				prin1[current]='0';
				current++;
			}
			else if(remote_status==224)
			{
				status=0;
				prin1[current]='.';
				current++;
			}
			else if(remote_status==2)
			{
				if(new_duty<0 || new_duty >100)
				{
					LCD_ShowString(30,230,200,16,16,"Input Error!");
					delay_ms(2000);
					_show_info();
					break;
				}
				else
				{
					duty=new_duty;
					PWMMODE_84psc(frequency,duty);
					_show_info();
					break;
				}
			}
			else if(remote_status==82)
			{
				if(current==0)
				{
					continue;
				}
				if(status)
				{
					new_duty=(int)(new_duty/10);
					current--;
					prin1[current]=0;
				}
				else
				{
					if(position>0.9)
					{
						position=1.0;
						status=1;
						current--;
						prin1[current]=0;
					}
					else
					{
						current--;
						new_duty-=(prin1[current]-'0')*position;
						position*=10;
						prin1[current]=0;
					}
				}
				_show_info();
				LCD_ShowString(30,150,220,16,16,"Please enter new duty!");
			}
			LCD_ShowString(30,190,200,16,16,prin1);
			remote_status=Remote_Scan();
			delay_ms(1000);
			remote_status=Remote_Scan();
		}
		else
		{
			delay_ms(50);
		}
	}
	
}





