#include "pid.h"


int myabs(int a)
{ 		   
	  int temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}

float pwm_control(float pwm)
{
    if(pwm>99)
        pwm=99;
    else if(pwm<-99)
        pwm=-99;
    return pwm;
}

float limit(float now,float tar)
{
    if(now>tar)
        now=tar;
    if(now<-tar)
        now=-tar;
    return now;
}



float i_Err_l=0,i_last_err_l=0,i_next_err_l=0,i_pwm_l=0,i_add_l=0,i_p_l=0.3,i_i_l=0.5,i_d_l=0.3;//80speedi_p=3.5,i_i_l=1.2,i_d_l=0
float i_pid_l(float now_speed,float tar_speed)
{		
    i_Err_l=tar_speed-now_speed;
//	  if(i_Err_l<=1)
//			i_i_l=0.01;
//		else
//			i_i_l=0.8;
    i_add_l=i_p_l*(i_Err_l-i_last_err_l)+i_i_l*(i_Err_l)+i_d_l*(i_Err_l+i_next_err_l-2*i_last_err_l);
    i_pwm_l+=i_add_l;
    i_pwm_l=pwm_control(i_pwm_l);
    i_next_err_l=i_last_err_l;
    i_last_err_l=i_Err_l;
    return i_pwm_l;
}

float i_Err_r=0,i_last_err_r=0,i_next_err_r=0,i_pwm_r=0,i_add_r=0,i_p_r=0.3,i_i_r=0.5,i_d_r=0.3;//80speedi_p=3.5,i_i_r=1.2,i_d_r=0
float i_pid_r(float now_speed,float tar_speed)
{		
    i_Err_r=tar_speed-now_speed;
//	  if(i_Err_r<=1)
//			i_i_r=0.01;
//		else
//			i_i_r=0.8;
    i_add_r=i_p_r*(i_Err_r-i_last_err_r)+i_i_r*(i_Err_r)+i_d_r*(i_Err_r+i_next_err_r-2*i_last_err_r);
    i_pwm_r+=i_add_r;
    i_pwm_r=pwm_control(i_pwm_r);
    i_next_err_r=i_last_err_r;
    i_last_err_r=i_Err_r;
    return i_pwm_r;
}
/***********************************************************************************************************/
//位置式pid位置控制
float p_Err_l=0,p_last_err_l=0,Integral_l=0,p_pwm_l=0,p_p_l=0.085,p_i_l=0,p_d_l=0.01;
float p_pid_l(int now_position,int tar_position)
{
	  
    p_Err_l=tar_position-now_position;
    if(myabs(p_Err_l)<5)
        p_p_l=0;
    else 
        p_p_l=0.085;
//		Integral_l+=p_Err_l;
    p_pwm_l=p_p_l*p_Err_l+p_i_l*Integral_l+p_d_l*(p_Err_l-p_last_err_l);
    p_pwm_l=pwm_control(p_pwm_l);
    p_last_err_l=p_Err_l;
    return p_pwm_l;
}
float p_Err_r=0,p_last_err_r=0,Integral_r=0,p_pwm_r=0,p_p_r=0.085,p_i_r=0,p_d_r=0;
float p_pid_r(int now_position,int tar_position)
{
	  
    p_Err_r=tar_position-now_position;
    if(myabs(p_Err_r)<5)
        p_p_r=0;
    else 
        p_p_r=0.085;
//		Integral_r+=p_Err_r;
    p_pwm_r=p_p_r*p_Err_r+p_i_r*Integral_r+p_d_r*(p_Err_r-p_last_err_r);
    p_pwm_r=pwm_control(p_pwm_r);
    p_last_err_r=p_Err_r;
    return p_pwm_r;
}
