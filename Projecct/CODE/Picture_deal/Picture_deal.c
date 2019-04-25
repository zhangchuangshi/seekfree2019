#include "Picture_deal.h"
#define ROW              60 //188            
#define COL              128//120  


extern volatile int threshold;
extern int16 x_x;
int16 beacon_x=0;
int16 beacon_y=0;
int16 twinkle_delay=0;


int16 Coordinate_Filter(int16 Channal,int16 Data_In)
{
	static int16	Data[3][3]={0};
	int16 Temp=0;
	Data[Channal][0]=Data[Channal][1];
	Data[Channal][1]=Data[Channal][2];
	Data[Channal][2]=Data_In;
	Temp=PaiXu(Data[Channal][0],Data[Channal][1],Data[Channal][2]);
	
	return Temp;
}

int16 X1[COL+20]= {0};
int16 Y1[COL+20]= {0};
u16 total_count = 0;

void Scan_Point(void)
{	   
	int16 i = 0, j = 0;
	int16 X=0,Y=0;
	uint8 pot_1=0;
	uint8 pot_2=0;
	int16 count_x = 0;
	int16 first = 1;
	u8 flag = 0;
	u8 total_flag = 0;
	u16 total = 0;
	static char delay_flag=0;
	for(u8 z =0;z<COL+20;z++)
	{
		X1[z]=0;
		Y1[z]=0;
	}
	
	for( i =0; i<ROW;i++)
	{				
		for(j =0; j < COL;j++)
		{
			if(image[i][j] > threshold)                //软件二值化，得到白点
			{  
				count_x++;                          //白点列统计
			}
			else 
			{
				if(count_x>=2)
				{
				    X = j-count_x/2;   //一块白色区域的中点位置
				    Y = i;
				    if(first==1)
				    {
							X1[pot_1++] =X;
							Y1[pot_2++] =Y;
							first = 2;
				    }

				    else if (Error(X,X1[pot_1-1])<4&&Error(Y,Y1[pot_1-1])<=2)
				    {
							X1[pot_1++] =X;
							Y1[pot_2++] =Y;
							flag =1;
				    } 
				}
					count_x = 0;
			}
		}
	}

	if(flag ==1)  //找到信标灯
	{
		delay_flag=1;
		beacon_x=X1[pot_1>>1];
		beacon_y=Y1[pot_2>>1];
	}
	else if(delay_flag == 1 && flag ==0)
	{
		beacon_x+=COL/2;
		twinkle_delay=10;//结合Time.c里面的注释看这句话就懂了
		delay_flag=0;
		total_flag =1;
	}
	else
	{
		if(twinkle_delay == 0)
		{
			beacon_x = 0;
			beacon_y = 0;
			total_flag =0;
		}
		else
		{
			beacon_x+=COL/2;
			total_flag =1;
		}
	}
	
	beacon_x -= COL/2;
	beacon_x=limit_s16(beacon_x,-COL/2,COL/2);
	beacon_y=limit_s16(beacon_y,0,60);
	beacon_x =Coordinate_Filter(0,beacon_x);
	beacon_y =Coordinate_Filter(1,beacon_y);
	if(beacon_x == -COL/2 && beacon_y == 0)
		beacon_x = 0;

	if(beacon_x ==0 && beacon_y==0)
	{
		total_count = 0;
	}
	else                                     //很明显，当摄像头看到的白点少时，是在远处，看到的白点多时，灯在近处，所以这句话判断白点数，以此来判断灯的远近
	{
		for(int m =0; m<ROW;m++)	
			for(int n =0; n < COL;n++)
			{
				if(image[m][n] > threshold)                //软件二值化，得到白点
				{  
					total++;                          //白点列统计
				}
			}
		if(total_flag == 0)
			total_count = total;
	}
	total_count =Coordinate_Filter(2,total_count);
x_x=beacon_x;
}
//检测信标灯的远近和有无
unsigned char  Judge_distance(int16 x,int16 y)   //传信标灯的坐标进去
{
	static u16 beacon_maybe_blocked =0;  //灯可能被挡住  因为车的高度就20cm，灯的高度是12cm，所以如果在一条直线上有多个灯，可能闪烁的信标灯会被其他灯挡住，导致摄像头看不到，这时候当连续原地转圈还找不到的话，就使车横移
	if((x ==0 && y==0 )|| (x>=58 && x<=-58))   //如果没有找到灯，或者找到的灯太偏离中线
	{
		beacon_maybe_blocked ++;
		if(beacon_maybe_blocked >=280 && beacon_maybe_blocked <= 320) //转圈2秒后，平移0.3秒
		{
			return FOREC_RAMPAGE;  //强制横移 FOREC_RAMPAGE
		}
		else
		{
			if(beacon_maybe_blocked > 320)
				beacon_maybe_blocked =0;
			return roll;   //没有时原地打转
		}
	}
	else
	{
		beacon_maybe_blocked=0;
		if(total_count<16)                          //参数需要整定
			return far;
		if(total_count<65 && total_count>=16)
		{
			return near;
		}
		if(total_count>=65 && total_count<=95)	
		{
			return very_near;
		}
		if(total_count>95)	
		{
			return very_very_near;
		}
	}
	return 0;   //不加这句话有警告
}

