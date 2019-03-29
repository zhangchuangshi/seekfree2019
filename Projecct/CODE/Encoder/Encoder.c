#include "Encoder.h"


int16 Left_rear_speed;/*右前轮速度*/
int16 Left_front_speed;/*左前轮速度*/
int16 Right_front_speed;/*右前轮速度*/
int16 Right_rear_speed;/*右后轮速度*/

void Encoder_Init()
{
	ctimer_count_init(TIMER0_COUNT1_A2);
	ctimer_count_init(TIMER3_COUNT0_A4);
	gpio_init(A3,GPI,0,PULLUP);
  gpio_init(A5,GPI,0,PULLUP);
	Pid_Init();
}


/*速度采集函数，定时器内运行*/
void speed_get(void)
{ 
			//ctimer_count_clean(TIMER0_COUNT0_A1);
	if(gpio_get(A3))Left_front_speed=ctimer_count_read(TIMER0_COUNT1_A2); 
		else  Left_front_speed=-ctimer_count_read(TIMER0_COUNT1_A2); 
	 
	if(gpio_get(A5))Right_front_speed=-ctimer_count_read(TIMER3_COUNT0_A4); 
		else  Right_front_speed=ctimer_count_read(TIMER3_COUNT0_A4); 
	 
	
	ctimer_count_clean(TIMER0_COUNT1_A2);
	ctimer_count_clean(TIMER3_COUNT0_A4);

}


/*二通道速度滤波函数*///这是滤波部分，在程序里可加可不加，我没加，加了也可以。但是看不出明显效果
int16 Speed_Filter(int16 Channal,int16 Data_In)
{
	static int16	Data[2][3]={0};
	int16 Temp=0;
	Data[Channal][0]=Data[Channal][1];
	Data[Channal][1]=Data[Channal][2];
	Data[Channal][2]=Data_In;
	Temp=PaiXu(Data[Channal][0],Data[Channal][1],Data[Channal][2]);
	
	return Temp;
}
