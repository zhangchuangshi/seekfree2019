#include "headfile.h"

#define far 1 
#define near 2
#define very_near 3
#define very_very_near 6
#define roll 4  //未找到灯
#define FOREC_RAMPAGE 5  //强制横行

void Scan_Point();
unsigned char  Judge_distance(int16 x,int16 y);
