#include "pid.h"
#include "ds18b20.h"
FUZZYPID pid;
FUZZYPID *vPID = &pid;
const int ruleKp[7][7] = {{PB, PB, PM, PM, PS, ZO, ZO}, {PB, PB, PM, PS, PS, ZO, NS}, {PM, PM, PM, PS, ZO, NS, NS}, {PM, PM, PS, ZO, NS, NM, NM}, {PS, PS, ZO, NS, NS, NM, NM}, {PS, ZO, NS, NM, NM, NM, NB}, {ZO, ZO, NM, NM, NM, NB, NB}};
const int ruleKi[7][7] = {{NB, NB, NM, NM, NS, ZO, ZO}, {NB, NB, NM, NS, NS, ZO, ZO}, {NB, NM, NS, NS, ZO, PS, PS}, {NM, NM, NS, ZO, PS, PM, PM}, {NM, NS, ZO, PS, PS, PM, PB}, {ZO, ZO, PS, PS, PM, PB, PB}, {ZO, ZO, PS, PM, PM, PB, PB}};
const int ruleKd[7][7] = {{PS, NS, NB, NB, NB, NM, PS}, {PS, NS, NB, NM, NM, NS, ZO}, {ZO, NS, NM, NM, NS, NS, ZO}, {ZO, NS, NS, NS, NS, NS, ZO}, {ZO, ZO, ZO, ZO, ZO, ZO, ZO}, {PB, NS, PS, PS, PS, PS, PB}, {PB, PM, PM, PM, PS, PS, PB}};
void pid_init(float set_temperatrue)
{
  pid.set_temperature = set_temperatrue;
  pid.out = 0.0;
  pid.error = 0.0;
  pid.lasterror = 0.0;
  pid.preerror = 0.0;
  pid.kp = KP;
  pid.ki = KI;
  pid.kd = KD;
  pid.maximum=10;
  pid.minimum=-20;
  pid.maxdKp=200;
  pid.mindKd=-200;
  pid.maxdKi=10;
  pid.mindKi=-10;
  pid.maxdKd=50;
  pid.mindKd=-50;
  pid.qKp=pid.qKd=pid.qKi=1;
}

float pid_realise() //实现pid
{
  float increment; //增量
  float temperature = (float)DS18B20_Get_Temp() / 10;
  pid.error = pid.set_temperature - temperature;

  increment = pid.kp * (pid.error - pid.lasterror) + pid.ki * pid.error + pid.kd * (pid.error - 2 * pid.lasterror + pid.preerror); //增量计算公式

  pid.out += increment;

  if (pid.out >= 500)
  {
    pid.out = 500;
  }
  else if (pid.out < 0)
  {
    pid.out = 0;
  }

  pid.preerror = pid.lasterror; //下一次迭代
  pid.lasterror = pid.error;
	FuzzyComputation(temperature);
  return pid.out;
}

static void LinearQuantization(float pv, float *qValue)
{

  float thisError;

  float deltaError;

  thisError = vPID->set_temperature - pv; //计算偏差值

  deltaError = thisError - vPID->lasterror; //计算偏差增量

  qValue[0] = 6.0 * thisError / (vPID->maximum - vPID->minimum);

  qValue[1] = 3.0 * deltaError / (vPID->maximum - vPID->minimum);
}

static void CalcMembership(float *ms, float qv, int *index)
{

  if ((qv >= -NB) && (qv < -NM))

  {

    index[0] = 0;

    index[1] = 1;

    ms[0] = -0.5 * qv - 2.0; //y=-0.5x-2.0

    ms[1] = 0.5 * qv + 3.0; //y=0.5x+3.0
  }

  else if ((qv >= -NM) && (qv < -NS))

  {

    index[0] = 1;

    index[1] = 2;

    ms[0] = -0.5 * qv - 1.0; //y=-0.5x-1.0

    ms[1] = 0.5 * qv + 2.0; //y=0.5x+2.0
  }

  else if ((qv >= -NS) && (qv < ZO))

  {

    index[0] = 2;

    index[1] = 3;

    ms[0] = -0.5 * qv; //y=-0.5x

    ms[1] = 0.5 * qv + 1.0; //y=0.5x+1.0
  }

  else if ((qv >= ZO) && (qv < PS))

  {

    index[0] = 3;

    index[1] = 4;

    ms[0] = -0.5 * qv + 1.0; //y=-0.5x+1.0

    ms[1] = 0.5 * qv; //y=0.5x
  }

  else if ((qv >= PS) && (qv < PM))

  {

    index[0] = 4;

    index[1] = 5;

    ms[0] = -0.5 * qv + 2.0; //y=-0.5x+2.0

    ms[1] = 0.5 * qv - 1.0; //y=0.5x-1.0
  }

  else if ((qv >= PM) && (qv <= PB))

  {

    index[0] = 5;

    index[1] = 6;

    ms[0] = -0.5 * qv + 3.0; //y=-0.5x+3.0

    ms[1] = 0.5 * qv - 2.0; //y=0.5x-2.0
  }
}

static void FuzzyComputation(float pv)
{

  float qValue[2] = {0, 0}; //偏差及其增量的量化值

  int indexE[2] = {0, 0}; //偏差隶属度索引

  float msE[2] = {0, 0}; //偏差隶属度

  int indexEC[2] = {0, 0}; //偏差增量隶属度索引

  float msEC[2] = {0, 0}; //偏差增量隶属度

  float qValueK[3];

  LinearQuantization(pv, qValue);

  CalcMembership(msE, qValue[0], indexE);

  CalcMembership(msEC, qValue[1], indexEC);

  qValueK[0] = msE[0] * (msEC[0] * ruleKp[indexE[0]][indexEC[0]] + msEC[1] * ruleKp[indexE[0]][indexEC[1]])

               + msE[1] * (msEC[0] * ruleKp[indexE[1]][indexEC[0]] + msEC[1] * ruleKp[indexE[1]][indexEC[1]]);

  qValueK[1] = msE[0] * (msEC[0] * ruleKi[indexE[0]][indexEC[0]] + msEC[1] * ruleKi[indexE[0]][indexEC[1]])

               + msE[1] * (msEC[0] * ruleKi[indexE[1]][indexEC[0]] + msEC[1] * ruleKi[indexE[1]][indexEC[1]]);

  qValueK[2] = msE[0] * (msEC[0] * ruleKd[indexE[0]][indexEC[0]] + msEC[1] * ruleKd[indexE[0]][indexEC[1]])

               + msE[1] * (msEC[0] * ruleKd[indexE[1]][indexEC[0]] + msEC[1] * ruleKd[indexE[1]][indexEC[1]]);

  vPID->kp += LinearRealization(vPID->maxdKp, vPID->mindKp, qValueK[0]);

  vPID->ki += LinearRealization(vPID->maxdKi, vPID->mindKi, qValueK[1]);

  vPID->kd += LinearRealization(vPID->maxdKd, vPID->mindKd, qValueK[2]);
}

static float LinearRealization(float max, float min, float value)
{
  return (max - min) * (value + 6) / 12 + min;
}