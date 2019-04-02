/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2018,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：179029047
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file       		main
 * @company	   		成都逐飞科技有限公司
 * @author     		逐飞科技(QQ3184284598)
 * @version    		查看LPC546XX_config.h文件内版本宏定义
 * @Software 		IAR 7.8 or MDK 5.24a
 * @Target core		LPC54606J512BD100
 * @Taobao   		https://seekfree.taobao.com/
 * @date       		2018-05-24
 ********************************************************************************************************************/

#include "headfile.h"


int16 Left_front_goalspeed;
int16 Right_front_goalspeed;
int16 Left_rear_goalspeed;
int16 Right_rear_goalspeed;

//----------------------------------------------------
//----------------------------------------------------
volatile int threshold=0;
uint8 min_gray=120;	
int flag2=0;
int flag1=0;
int num;

#define a_PARAMETER          (0.09f)               
#define b_PARAMETER          (0.09f) 
void McNamm_wheel_control(float Vx,float Vy,float Vz);

void pwm_init();

 //----------------------------------------------------
int main(void)
{   
    get_clk();
    gpio_init(B2,GPO,1,PULLUP);
		uart_init(USART_0,115200,UART0_TX_A25,UART0_RX_A24); 
	  pwm_init();
	  lcd_init();  
		gpio_set(B2,0);
    mrt_pit_init_ms(MRT_CH0,5);
	  Encoder_Init();
	  camera_init(); 		
	  EnableInterrupts;
	  uart_putchar(USART_0,0xA5);  
	  McNamm_wheel_control(0,0,0);
    while(1)
    {

			
		if(mt9v032_finish_flag)
		{
			//mrt_start(MRT_CH0);
			threshold=MyOSTU(120,60,*image);
				
				if(threshold<min_gray)threshold=min_gray;
			
			lcd_displayimageOUST(image[0],MT9V032_W,MT9V032_H,threshold);
				//lcd_displayimage032(image[0],MT9V032_W,MT9V032_H);
			
			/*if(flag1==0)lcd_displayimage032(image[0],MT9V032_W,MT9V032_H);
			else {
				lcd_displayimageOUST(image[0],MT9V032_W,MT9V032_H,threshold);
			}//*/
			Scan_Point();
			mt9v032_finish_flag = 0;		
			gpio_toggle(B2);
		}//


    }
}

//----------------------------------------------------------------
//----------------------------------------------------------------
void pwm_init()
{
    sct_pwm_init(SCT0_OUT2_A19,50,0);
		sct_pwm_init(SCT0_OUT3_A31,50,0);
	
		sct_pwm_init(SCT0_OUT4_A23,50,0);
	  sct_pwm_init(SCT0_OUT5_A26,50,0);
	
		sct_pwm_init(SCT0_OUT6_A27,50,0);
	  sct_pwm_init(SCT0_OUT7_A28,50,0);
	
		sct_pwm_init(SCT0_OUT8_A29,50,0);
	  sct_pwm_init(SCT0_OUT9_A30,50,0);
}
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
/*
功能:PWM占空比赋值
入口参数:无
返回值:无
解释:  此处有BUG   正确顺序是  左前--右前-左后--右后
*/
void control(int Right_rear_goalspeed,int Left_rear_goalspeed,int Right_front_goalspeed,int Left_front_goalspeed)//Left_rear_goalspeed
{
	if (Left_front_goalspeed>=0){//左前
		sct_pwm_duty(SCT0_OUT2_A19,Left_front_goalspeed); 
		sct_pwm_duty(SCT0_OUT3_A31,0); 
	}else{
		Left_front_goalspeed=~Left_front_goalspeed+1;
		sct_pwm_duty(SCT0_OUT2_A19,0); 
		sct_pwm_duty(SCT0_OUT3_A31,Left_front_goalspeed); 	
	}
	if (Right_front_goalspeed>=0){//右前
	  sct_pwm_duty(SCT0_OUT4_A23,Right_front_goalspeed); 
		sct_pwm_duty(SCT0_OUT5_A26,0); 	
	}else{
		Right_front_goalspeed=~Right_front_goalspeed+1;
	  sct_pwm_duty(SCT0_OUT4_A23,0); 
		sct_pwm_duty(SCT0_OUT5_A26,Right_front_goalspeed); 		
	}
	if (Left_rear_goalspeed>=0){//左后
	  sct_pwm_duty(SCT0_OUT6_A27,Left_rear_goalspeed); 
		sct_pwm_duty(SCT0_OUT7_A28,0);
	}else{
		Left_rear_goalspeed=~Left_rear_goalspeed+1;
	  sct_pwm_duty(SCT0_OUT6_A27,0); 
		sct_pwm_duty(SCT0_OUT7_A28,Left_rear_goalspeed);
	}
		if (Right_rear_goalspeed>=0){//右后
		sct_pwm_duty(SCT0_OUT8_A29,0); 
		sct_pwm_duty(SCT0_OUT9_A30,Right_rear_goalspeed);
		}else{
		Right_rear_goalspeed=~Right_rear_goalspeed+1;
		sct_pwm_duty(SCT0_OUT8_A29,Right_rear_goalspeed); 
		sct_pwm_duty(SCT0_OUT9_A30,0);
		}
}
//----------------------------------------------------------------
/*编码器
volatile int num=0;

ctimer_count_init(TIMER0_COUNT1_A2);

num = ctimer_count_read(TIMER0_COUNT1_A2);
*/


/*led*
 gpio_init(B2,GPO,1,PULLUP);

gpio_set(B2,0);
gpio_toggle(B2); //翻转
*/
/* PWM
		int pwm=0,flag=0;

		sct_pwm_init(SCT0_OUT2_A19,50,0);
		sct_pwm_init(SCT0_OUT3_A31,50,0);
		//sct_pwm_duty(SCT0_OUT2_A19,5000); 




			if(flag==0)pwm++;
			if(flag==1)pwm--;
			if(pwm>=10000) flag=1;
			if(pwm==0) flag=0;
			sct_pwm_duty(SCT0_OUT3_A31,pwm); 
			systick_delay_ms(10);
*/


