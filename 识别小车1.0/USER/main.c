#include <stm32f10x.h>
#include "delay.h"
#include "oled.h"
#include "sys.h"
#include "motor.h"
#include "led.h"
#include "Key.h"
#include "pid.h"
#include "serial.H"
#include "xunji.h"

#define distance 4000
#define Turn_distance 230
#define Turn_speed 2
#define Run_Speed 2
extern uint16_t Dead_Voltage;
extern uint16_t  k210_data;      
extern uint16_t Serial_RxPacket[5];
extern uint8_t k210_flag;
float now_speed_left,now_speed_right;
float pwm_left,pwm_right;
float tar_speed_left=Run_Speed,tar_speed_right=Run_Speed;
int16_t now_position_left,now_position_right,save_position_left,save_position_right;
int16_t tar_psoition_left=distance,tar_psoition_right=distance;
uint8_t turn_flag,Led_flag,Turn_Stop_flag,move_flag;
uint8_t xunji_vaule[5];
uint16_t time,Time_1000ms;
uint16_t Oled_Slow_Down;

uint8_t Key_Val=0,Key_Down=0,Key_Old=0,Key_Up=0;


void Xunji()
{
	xunji_vaule[0]=Read_Xunji(xunji_1);
	xunji_vaule[1]=Read_Xunji(xunji_2);
	xunji_vaule[2]=Read_Xunji(xunji_3);
	xunji_vaule[3]=Read_Xunji(xunji_4);
	xunji_vaule[4]=Read_Xunji(xunji_5);
	
	if(Read_Xunji(xunji_1)==1&&Read_Xunji(xunji_5)==1)
	{
			if(Read_Xunji(xunji_2)==0&&Read_Xunji(xunji_3)==1&&Read_Xunji(xunji_4)==1)//左转
			{
				tar_speed_right+=0.02;
			}
			if(Read_Xunji(xunji_2)==0&&Read_Xunji(xunji_3)==0&&Read_Xunji(xunji_4)==1)//左转
			{
				tar_speed_right+=0.01;
			}
			if(Read_Xunji(xunji_2)==1&&Read_Xunji(xunji_3)==0&&Read_Xunji(xunji_4)==1)//直行
			{
				tar_speed_right=Run_Speed;
				tar_speed_left=Run_Speed;
			}
			if(Read_Xunji(xunji_2)==1&&Read_Xunji(xunji_3)==0&&Read_Xunji(xunji_4)==0)//右转
			{	
				tar_speed_left+=0.01;
			}
			if(Read_Xunji(xunji_2)==1&&Read_Xunji(xunji_3)==1&&Read_Xunji(xunji_4)==0)//右转
			{	
				tar_speed_left+=0.02;
			}
  }
	else 
	{	
			if((Read_Xunji(xunji_1)==0)&&(turn_flag==0)&&(Turn_Stop_flag==0))//左直角转
			{
				save_position_left=now_position_left;
				save_position_right=now_position_right;
				now_position_left=0;
				now_position_right=0;
				tar_speed_left=Turn_speed;
				tar_speed_right=Turn_speed;
				tar_psoition_left=-Turn_distance;
				tar_psoition_right=Turn_distance;				
				turn_flag=1;
				Turn_Stop_flag=1;	
			}
			if((Read_Xunji(xunji_5)==0)&&(turn_flag==0)&&(Turn_Stop_flag==0))//右直角转
			{		
				save_position_left=now_position_left;
				save_position_right=now_position_right;
				now_position_left=0;
				now_position_right=0;
				tar_speed_left=Turn_speed;
				tar_speed_right=Turn_speed;
				tar_psoition_left=Turn_distance;
				tar_psoition_right=-Turn_distance;				
				turn_flag=1;
				Turn_Stop_flag=1;				
			}			
	}
		if(myabs(tar_psoition_right-now_position_right)<5&&turn_flag==1)
		{
			now_position_left=save_position_left;
			now_position_right=save_position_left;	
			tar_psoition_left=distance;
			tar_psoition_right=distance;	
			tar_speed_left=Run_Speed;
			tar_speed_right=Run_Speed;
			turn_flag=0;	
		}
}

void Key()
{
	Key_Val = Key_GetNum();
	Key_Down = Key_Val & (Key_Old ^ Key_Val);
	Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
	Key_Old = Key_Val;
	
	switch(Key_Down)
	{
		case 1:	
			tar_speed_right+=5;
			tar_psoition_right+=400;
		break;
		case 2:
		
			tar_speed_right-=5;
			tar_psoition_right-=400;
		break;
		case 3:
			move_flag=~move_flag;
		break;
	}
	
}


void Oled_disp()
{

	OLED_ShowTrace(xunji_vaule);
	OLED_ShowPWM(pwm_left);
	OLED_ShowNum(1, 1, tar_psoition_left, 1);
	OLED_ShowNum(36, 1, now_position_left,1);
	OLED_ShowNum(1, 2, tar_speed_right, 1);
	OLED_ShowNum(36, 2, now_speed_left,1);
	
	OLED_ShowPWM(pwm_right);
	OLED_ShowNum(1, 4, tar_psoition_right, 1);
	OLED_ShowNum(36, 4, now_position_right,1);
	OLED_ShowNum(1, 5, tar_speed_right, 1);
	OLED_ShowNum(36, 5, now_speed_left,1);
	
	if(Led_flag)
		LED_ON;
	else
		LED_OFF;
	            
}
int main()
{
	delay_init(); 	                        
	LED_Init();															
 	motor_init();                          
 	Serial_Init();    
	OLED_Init();   	
	Key_Init();
	Xunji_Init();
	
	while(1)
	{	
		Key();
		Oled_disp();
	}
}

void TIM1_UP_IRQHandler(void)//0.1ms定时
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        // 这里写你的中断服务代码
        // ...
			if(++Oled_Slow_Down == 1000)
			{		
				Led_flag=~Led_flag;		
				Oled_Slow_Down = 0;
				//printf("%.2f,%.2f,%d,%d,%.2f,%d\n",now_speed_right,tar_speed_right,now_position_right,tar_psoition_right,pwm_right,Receive_Data[0]);
			}
			if(Turn_Stop_flag)
			{
				tar_speed_left=0;
				tar_speed_right=0;
				if(++Time_1000ms==10000)
				{
					tar_speed_left=Turn_speed;
					tar_speed_right=Turn_speed;
					Time_1000ms=0;
					Turn_Stop_flag=0;
				}			
			}
			if(++time==100)
			{		
				if(move_flag)
				{
					Xunji();
					now_speed_left=read_encoder3();//左轮前进
					now_position_left+=now_speed_left;
					pwm_left=p_pid_l(now_position_left,tar_psoition_left);
					pwm_left=limit(pwm_left,tar_speed_left);
					if(myabs(tar_psoition_left-now_position_left)<5)
					 Set_PWM_left(0);
					else
					{
						pwm_left=i_pid_l(now_speed_left,pwm_left);
						Set_PWM_left(pwm_left);
					}
					now_speed_right=read_encoder2();  //右轮前进
					now_position_right+=now_speed_right;
					pwm_right=p_pid_r(now_position_right,tar_psoition_right);
					pwm_right=limit(pwm_right,tar_speed_right);
					if(myabs(tar_psoition_right-now_position_right)<5)
					 Set_PWM_right(0);
					else
					{
						pwm_right=i_pid_r(now_speed_right,pwm_right);
						Set_PWM_right(pwm_right);
					}
				}
				time=0;
			}
        // 清除中断标志位
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}
