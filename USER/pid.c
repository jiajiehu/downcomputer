#include"pid.h"
#include"ds18b20.h"
Pid pid;
void pid_init(float set_temperatrue)
{
    pid.set_temperature = set_temperatrue;
    pid.out = 0.0;
    pid.error = 0.0;
    pid.error_next = 0.0;
    pid.error_last = 0.0;
    //可调节PID 参数。使跟踪曲线慢慢接近阶跃函数200.0 // 
    pid.kp = KP;
    pid.ki = KI;
    pid.kd = KD;
}


float pid_realise()//实现pid  
{
	float increment;//增量
    pid.error = pid.set_temperature - (float)DS18B20_Get_Temp()/10;
    
     
    increment = pid.kp*(pid.error-pid.error_next)+pid.ki*pid.error+pid.kd*(pid.error-2*pid.error_next+pid.error_last);//增量计算公式  
    
    pid.out+= increment;
    
    if(pid.out>=500){
        pid.out=500;
    }else if(pid.out<0){
        pid.out=0;
    }

    pid.error_last = pid.error_next;//下一次迭代  
    pid.error_next = pid.error;
    return pid.out; 
    
}