#ifndef __PID_H
#define __PID_H

float i_pid_l(float now_speed,float tar_speed);
float i_pid_r(float now_speed,float tar_speed);
float p_pid_l(int now_position,int tar_position);
float p_pid_r(int now_position,int tar_position);
float limit(float now,float tar);
int myabs(int a);
                              
#endif
