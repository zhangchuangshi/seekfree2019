#include "headfile.h"
#include "SEEKFREE_MT9V032.h"


#define a_PARAMETER          (0.09f)               
#define b_PARAMETER          (0.09f) 

extern int16 beacon_x;
extern int16 beacon_y;
uint8 avoid_x;
uint8 avoid_y;
u8 basic_speed=100;//100
u8 set_speed_l;
int set_speed_s;
u8 Rotate_speed=120;//120
int16 excursion_x;  //信标灯坐标偏移
int16 excursion_y;
u8 No_Barrier_flag ;
const u8 COL_2= 98;
u8 excursion_flag;
u8 state_flag;
u8 delay_flag;
u8 Barrier_flag ;
u8 force_left =0;
u8 force_right = 0;

int16 Left_front_goalspeed;
int16 Right_front_goalspeed;
int16 Left_rear_goalspeed;
int16 Right_rear_goalspeed;
/*
功能:轮胎控制
入口参数: X x轴的速度(平移)  Y y轴的速度(直线速度) Z 左转右转
返回值:无
解释:
*/
void McNamm_wheel_control(float Vx,float Vy,float Vz)
{
	  Left_rear_goalspeed    = -Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
    Left_front_goalspeed   = +Vx+Vy-Vz*(a_PARAMETER+b_PARAMETER);
	  Right_front_goalspeed  = -Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
		Right_rear_goalspeed   = +Vx+Vy+Vz*(a_PARAMETER+b_PARAMETER);
    //control(Left_front_goalspeed,Right_front_goalspeed,Left_rear_goalspeed,Right_rear_goalspeed);//
}
void Speed_Conversion(unsigned char setspeedl,int setspeeds,int16 x,int16 y)
{

	float L =0;
	float vx=0,vy=0;
	int vz = 0;
	if(y == 60)
		y =0;
	L = sqrt(x*x+y*y);
	if(L != 0)
	{
		vx = (float)setspeedl*x/L;
		vy = (float)setspeedl*y/L;
	}
	else
	{
		vz = setspeeds;
	}
	McNamm_wheel_control(vx,vy,vz);
}
void Car_Control()
{
	int16 Barrier_X=0;
	int16 Barrier_Y =0;
	delay_flag = 0;
	Barrier_flag = 0;
	force_left =0;
	force_right = 0;
	Barrier_X = avoid_x-COL_2/2;  //当Barrier_X为-64 Barrier_Y为0时，表示没找到障碍物
	Barrier_Y = avoid_y;
	state_flag = Judge_distance(beacon_x,beacon_y);//beacon_x  beacon_y
	switch(state_flag)
	{
		case far :
			{
				if(Barrier_X ==-COL_2/2 && Barrier_Y ==0 && ((beacon_x !=0)||(beacon_y!=0)))  //车离亮着的信标灯远，且车附近没有障碍物 
				{
//					LED2_ON;
					if(beacon_x>=0)
					{
						set_speed_l=basic_speed;
						set_speed_s = 0;
						excursion_x = beacon_x-4;   
						excursion_y = beacon_y;			
					}		
					else
					{
						set_speed_l=basic_speed;
						set_speed_s = 0;
						excursion_x = beacon_x+4;   
						excursion_y = beacon_y;		
					}					
				}
				else if((Barrier_X !=-COL_2/2 || Barrier_Y !=0)&& ((beacon_x !=0)||(beacon_y!=0)))   //车离亮着的信标灯远，且车附近有障碍物 
				{
//					LED2_OFF;
					if(Barrier_X>16)
					{
						Barrier_flag =1;
						set_speed_l=basic_speed;
						set_speed_s = 0;
						excursion_x = -10;
						excursion_y = 58;
					}
					else if(Barrier_X<-16)
					{
						Barrier_flag =2;
						set_speed_l=basic_speed;
						set_speed_s = 0;
						excursion_x = 10;
						excursion_y = 58;					
					}
					else if(Barrier_X >= 0 &&Barrier_X <= 16 )    //障碍物坐标在中间稍微偏右
					{
						Barrier_flag =3;
						set_speed_l=basic_speed*0.85;
						set_speed_s = 0;
						excursion_x = -10;
						excursion_y = 59	;		
					}
					else
					{
						Barrier_flag =4;
						set_speed_l=basic_speed*0.85;
						set_speed_s = 0;
						excursion_x = 10;
						excursion_y = 59;	
					}	

				}
			}
			break;			
		case near :
			{
				if(beacon_x>=5 && beacon_x<=38)   //  5-25
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x-13;
					excursion_y = beacon_y ;
				}
				else if(beacon_x<=-5 &&  beacon_x>=-38)   //-5 ~ -25
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x+16;
					excursion_y = beacon_y ;
				}
				else if(beacon_x<=5 &&  beacon_x>=-5)    // -5 ~ 5
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x+18;
					excursion_y = beacon_y ;
				}
				else if(beacon_x> 38 )   //           大于25
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x-5;  
					excursion_y = beacon_y ;
					force_right = 1;
				}
				else                                   // 小于-25
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x+5;
					excursion_y = beacon_y ;
					force_left = 1;
				}
			}
			break;
					
		case very_near:
			{
				if(beacon_x>=0)
				{
					set_speed_l=basic_speed;     //给0.35减速
					set_speed_s = 0;
					excursion_x = beacon_x-24	 ;   //偏离-15
					excursion_y = beacon_y;
				}
				else
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x = beacon_x+26;
					excursion_y = beacon_y;
				}
			}
			break;
			case very_very_near:
			{
				if(beacon_x>=0)
				{
					set_speed_l=basic_speed;     //给0.35减速
					set_speed_s = 0;
					excursion_x = beacon_x-24	 ;   //偏离-15
					excursion_y = beacon_y;
				}
				else
				{
					set_speed_l=basic_speed;
					set_speed_s = 0;
					excursion_x =beacon_x+26;
					excursion_y = beacon_y;
				}
			}
			break;
		case roll:
			{
				set_speed_l=0;
				excursion_x = 0;   
				excursion_y = 0;
				set_speed_s = -Rotate_speed;
			}
			break;
		case FOREC_RAMPAGE:
			{
				set_speed_l=basic_speed;
				set_speed_s = 0;
				excursion_x = 1;
				excursion_y = 1;	
			}
			break;
			
		default:
			break;
	}
	Speed_Conversion(set_speed_l,set_speed_s,excursion_x,60-excursion_y);
	
}



