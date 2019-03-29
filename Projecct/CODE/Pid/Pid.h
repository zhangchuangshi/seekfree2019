#ifndef __PID_H
#define __PID_H

#include "headfile.h"


void Pid_Init(void);
int Dir_Pid(int midpoint,int error_act);
void PID_Control(_pid_t *pid, int16 actual, int16 set);

#endif

