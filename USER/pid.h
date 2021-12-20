#ifndef _PID_H
#define _PID_H
#define KP 4830.74
#define KI 70.01
#define KD 605.5
void pid_init(float temperature);//PID参数初始化 
float pid_realise();//实现PID算法   
typedef struct {
    float set_temperature;//设定温度 
    float out;//实际温度
    float error;//偏差  
    float error_next;//上一个偏差  
    float error_last;//上上一个偏差 
    float kp,ki,kd;//定义比例，积分，微分参数  
     
}Pid;


#endif