#ifndef _Car_Control_h
#define _Car_Control_h

//#include "Fivedir_key.h"
#include "common.h"
//#include "Variable.h"
//#include "McNamm_wheel_control.h"
#include "Picture_deal.h"
void Car_Control(void);
void McNamm_wheel_control(float Vx,float Vy,float Vz);
void Speed_Conversion(unsigned char setspeedl,int setspeeds,int16 x,int16 y);
	
#endif

