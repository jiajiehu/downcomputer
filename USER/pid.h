#ifndef _PID_H
#define _PID_H
#define KP 4830.74
#define KI 70.01
#define KD 605.5
#define NB -6
#define NM -4
#define NS -2
#define ZO 0
#define PS 2
#define PM 4
#define PB 6
// void pid_init(float temperature);//PID参数初始化 
// float pid_realise();//实现PID算法   
// typedef struct {
//     float set_temperature;//设定温度 
//     float out;//实际温度
//     float error;//偏差  
//     float error_next;//上一个偏差  
//     float error_last;//上上一个偏差 
//     float kp,ki,kd;//定义比例，积分，微分参数  
     
// }Pid;

typedef struct

{

  float set_temperature;        /*设定值*/

  float kp;                     /*比例系数*/

  float ki;                     /*积分系数*/

  float kd;                     /*微分系数*/

  float error;                  /*当前偏差*/

  float lasterror;              /*前一拍偏差*/

  float preerror;               /*前两拍偏差*/

  float out;                    /*输出值*/

  float maximum;                /*输出值的上限*/

  float minimum;                /*输出值的下限*/

 

  float maxdKp;                 /*Kp增量的最大限值*/

  float mindKp;                 /*Kp增量的最小限值*/

  float qKp;                    /*Kp增量的影响系数*/

  float maxdKi;                 /*Ki增量的最大限值*/

  float mindKi;                 /*Ki增量的最小限值*/

  float qKi;                    /*Ki增量的影响系数*/

  float maxdKd;                 /*Kd增量的最大限值*/

  float mindKd;                 /*Kd增量的最小限值*/

  float qKd;                    /*Kd增量的影响系数*/

}FUZZYPID;


/*线性量化操作函数,论域{-6，-5，-4，-3，-2，-1，0，1，2，3，4，5，6}*/

static void LinearQuantization(float pv,float *qValue);



/*隶属度计算函数*/

static void CalcMembership(float *ms,float qv,int * index);


/*解模糊化操作,根据具体的量化函数和隶属度函数调整*/

static void FuzzyComputation (float pv);

/*线性化*/
static float LinearRealization(float max,float min,float value);

#endif