#ifndef __ENCODER_H
#define __ENCODER_H

#include "headfile.h"



void speed_get(void);
int16 Speed_Filter(int16 Channal,int16 Data_In);
void Encoder_Init(void);

#endif

