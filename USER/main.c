#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "pwm.h"
#include "lcd.h"
#include "pid.h"
short temperature=200;  
float set_temperature=-10;
int main(void)
{ 
	u16 i=0;
	u8 t=0;
	short temp_temperature;
	float temp_set;			    
	pid_init(set_temperature);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(38400);//��ʼ�����ڲ�����Ϊ115200
 	TIM14_PWM_Init(500-1,84-1);	//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz.  	
	TIM3_Int_Init(500-1,8400-1);//pid timer
	//TIM_SetCompare1(TIM14,0);	//�޸ıȽ�ֵ���޸�ռ�ձ�
	LCD_Init();
	while(DS18B20_Init())	//DS18B20��ʼ��	
	{
		LCD_ShowString(30,130,200,16,16,"DS18B20 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}  
	LCD_ShowString(30,130,200,16,16,"DS18B20 OK");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	while(1)
	{	    	    
 		if(t%10==0)//ÿ100ms��ȡһ��
		{									  
			temperature=DS18B20_Get_Temp();	
			temp_temperature=temperature;
			LCD_ShowString(30+40,150,200,30,16,"The Current Temperature:");
			if(temperature<0)
			{
				LCD_ShowChar(30+40+200,150,'-',16,0);			//��ʾ����
				temp_temperature=-temperature;					//תΪ����
			}else 
			LCD_ShowChar(30+40+200,150,' ',16,0);			//ȥ������
			LCD_ShowNum(30+40+200+8,150,temp_temperature/10,2,16);	//��ʾ��������	
    	LCD_ShowChar(30+40+200+24,150,'.',16,0);		
   		LCD_ShowNum(30+40+200+32,150,temp_temperature%10,1,16);	//��ʾС������ 	
			LCD_ShowString(30+40,200,200,30,16,"The Target Temperature:");
			if(set_temperature<0)
			{
				LCD_ShowChar(30+40+200,200,'-',16,0);			//��ʾ����
				temp_set=-set_temperature;					//תΪ����
			}else 
			LCD_ShowChar(30+40+200,200,' ',16,0);			//ȥ������
			LCD_ShowNum(30+40+200+8,200,(unsigned short)(temp_set*10)/10,2,16);	//��ʾ��������	 
      LCD_ShowChar(30+40+200+24,200,'.',16,0);			
   		LCD_ShowNum(30+40+200+32,200,(unsigned short)(temp_set*10)%10,1,16);	//��ʾС������   	   
		}				   
	 	delay_ms(10);
		t++;
	}
}
