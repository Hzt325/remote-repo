#include "mpuexti.h"
#include "sys.h"
#include "mpu6050.h"   
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "OLED.h"
#include "delay.h"
#include "pid.h"
#include "motor.h"
#include "timer.h"
#include "led.h"



unsigned int Oled_Slow_Down;
float pitch,roll,yaw;        //ŷ����

int16_t now_speed_left,now_position_left,now_speed_right,now_position_right;
int16_t pwm_left,pwm_right;
int16_t tar_speed_left=20,tar_speed_right=10;
int16_t tar_psoition_left=4000,tar_psoition_right=4000;

void MPU_exti_init()
{
	EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);				  //ʹ�ܸ��ù���ʱ��
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;								  //PA12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 					    //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
 
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12); //��PA12ӳ�䵽�ж���12
	EXTI_InitStructure.EXTI_Line=EXTI_Line12;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;     //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	  	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 	//��ռ���ȼ�0 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			    //��Ӧ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						
	NVIC_Init(&NVIC_InitStructure);
	
}


/**********************************************************

					�ⲿ�ж���12�������PID�㷨�ڴ˺�����

          ע�������ϸ�֤���ݽ��պ�PID�����ͬ��

***********************************************************/ 
void EXTI15_10_IRQHandler(void) //�ⲿ�ж�(10ms)
{
	if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
	{ 	
		  if(++Oled_Slow_Down == 10)
			{
				Oled_Slow_Down = 0;			
			}
			
//				now_speed_left=read_encoder3();//����ǰ��
//				now_position_left+=now_speed_left;
//				pwm_left=p_pid(now_position_left,tar_psoition_left);
//				pwm_left=limit(pwm_left,tar_speed_left);
//			  if(myabs(tar_psoition_left-now_position_left)<5)
//				 Set_PWM_left(0);
//			  else
//		  	{
//					pwm_left=i_pid(now_speed_left,pwm_left);
//					Set_PWM_left(pwm_left);
//		  	}
//				
//				now_speed_right=read_encoder2();//����ǰ��
//				now_position_right+=now_speed_right;
//				pwm_right=p_pid(now_position_right,tar_psoition_right);
//				pwm_right=limit(pwm_right,tar_speed_right);
//			  if(myabs(tar_psoition_right-now_position_right)<5)
//				 Set_PWM_right(0);
//			  else
//		  	{
//					pwm_right=i_pid(now_speed_right,pwm_right);
//					Set_PWM_right(pwm_right);
//		  	}
		
	}
	
	EXTI_ClearITPendingBit(EXTI_Line12);           //���LINE0�ϵ��жϱ�־λ  
}
