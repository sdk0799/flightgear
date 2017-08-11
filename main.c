/*
 * main.c
 *
 *  Created on: 2017-8-9
 *      Author: wangbo
 */



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>//创建文件
#include <pthread.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>
/*转换int或者short的字节顺序，该程序arm平台为大端模式，地面站x86架构为小端模式*/
#include <byteswap.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "maintask.h"
#include "utility.h"
#include "udp.h"
#include "global.h"

#define TEST
#ifdef TEST
char udp_string[]="0123456";
#endif

T_GLOBAL  gblState;
T_AP2FG  ap2fg;
T_FG2AP fg2ap;
T_AP2FG  ap2fg_send;


T_AP2FG  ap2fg_recv;



double latitude;
double longitude;
double altitude;

int main()
{
	/*
	* 这一部分写程序的初始化
	*/

	//open_udp_dev(IP_SEND_TO, 49000, PORT_RECEIVE);
	open_udp_dev(IP_SEND_TO, PORT_SENT_TO, PORT_RECEIVE);

	/***************************/
	/***************************到此初始化部分结束**********************************************/

	printf("Enter the maintask...\n");
	init_maintask();


	pthread_t loopfast_pthrd = 0;
	pthread_t loopslow_pthrd = 0;
	static int sem_loopfast_cnt;
	static int sem_loopslow_cnt;
	int ret=0;

	/*
	* 初始化快循环信号量
	*/
	sem_init(&sem_loopfast,0,1);/*初始化时，信号量为1*/
	ret = pthread_create (&loopfast_pthrd,          //线程标识符指针
	NULL,                     //默认属性
	(void *)loopfast,         //运行函数
	NULL);                    //无参数
	if (0 != ret)
	{
		perror ("pthread create error\n");
	}

	/*
	* 初始化慢循环信号量
	*/
	sem_init(&sem_loopslow,0,1);
	ret = pthread_create (&loopslow_pthrd,          //线程标识符指针
												NULL,                     //默认属性
												(void *)loopslow,         //运行函数
												NULL);                    //无参数
	if (0 != ret)
	{
		perror ("pthread create error\n");
	}



	int seconds=0;
	int mseconds=MAINTASK_TICK_TIME_MS*(1e3);/*每个tick为20毫秒，也就是20000微秒*/
	struct timeval maintask_tick;


	static double delta_0=0.1;
	static double delta_1=0.1;
	static double delta_2=0.1;
	static double delta_3=0.1;
	ap2fg.throttle0 = 0.2;
	ap2fg.throttle1 = 0.3;
	ap2fg.throttle2 = 0.4;
	ap2fg.throttle3 = 0.5;

	/*
	* 开始maintask任务，maintask任务按最小的tick执行，周期时间为20ms，执行一次
	*/
	while (1)
	{
		maintask_tick.tv_sec = seconds;
		maintask_tick.tv_usec = mseconds;
		select(0, NULL, NULL, NULL, &maintask_tick);

		main_task.maintask_cnt++;//20ms这个计数加1

		/*loopfast 快循环*/
		if(0 == main_task.maintask_cnt%LOOP_FAST_TICK)
		{
		sem_getvalue(&sem_loopfast,&sem_loopfast_cnt);
		if(sem_loopfast_cnt<1)
		{
		sem_post (&sem_loopfast);/*释放信号量*/
		}
		}

		/*loopslow 慢循环*/
		if(0 == main_task.maintask_cnt%LOOP_SLOW_TICK)
		{
			sem_getvalue(&sem_loopslow,&sem_loopslow_cnt);
			if(sem_loopslow_cnt<1)
			{
			sem_post (&sem_loopslow);        /*释放信号量*/
			}

			//打印当前系统运行时间
			float system_running_time=0.0;
			system_running_time=clock_gettime_s();
			printf("系统从开启到当前时刻运行的时间%f[s]\n",system_running_time);

			/*
			* 可以直接把程序写在这里也可以写在loopslow函数里
			*/


			if(ap2fg.throttle0>=1||ap2fg.throttle0<=0.1)
			{
				delta_0=-delta_0;
			}
			if(ap2fg.throttle1>=1||ap2fg.throttle1<=0.1)
			{
				delta_1=-delta_1;
			}
			if(ap2fg.throttle2>=1||ap2fg.throttle2<=0.1)
			{
				delta_2=-delta_2;
			}
			if(ap2fg.throttle3>=1||ap2fg.throttle3<=0.1)
			{
				delta_3=-delta_3;
			}

			ap2fg.throttle0 = ap2fg.throttle0 +delta_0;
			ap2fg.throttle1 = ap2fg.throttle1 +delta_1;
			ap2fg.throttle2 = ap2fg.throttle2 +delta_2;
			ap2fg.throttle3 = ap2fg.throttle3 +delta_3;
//			ap2fg.throttle0 = 0.2;
//			ap2fg.throttle1 = 0.3;
//			ap2fg.throttle2 = 0.4;
//			ap2fg.throttle3 = 0.5;
#if 0
			ap2fg.latitude_deg = 40;
			ap2fg.longitude_deg = 117;
			ap2fg.altitude_ft = 30;
			ap2fg.altitude_agl_ft = 100;
			ap2fg.roll_deg = 2.0;
			ap2fg.pitch_deg = 0.0;
			ap2fg.heading_deg = 38.0;
#endif

	//		ap2fg.roll_deg = (ap2fg.throttle1 - ap2fg.throttle0)*10;
	//		ap2fg.pitch_deg = (ap2fg.throttle2 - ap2fg.throttle3)*10;
	//		ap2fg.heading_deg = (ap2fg.throttle0 + ap2fg.throttle2 - ap2fg.throttle1 - ap2fg.throttle3)*10;

			memcpy(&ap2fg_send,&ap2fg,sizeof(ap2fg));
			ap2fg_send.throttle0=hton_double(ap2fg_send.throttle0);
			ap2fg_send.throttle1=hton_double(ap2fg_send.throttle1);
			ap2fg_send.throttle2=hton_double(ap2fg_send.throttle2);
			ap2fg_send.throttle3=hton_double(ap2fg_send.throttle3);
#if 0
			ap2fg_send.latitude_deg=hton_double(ap2fg_send.latitude_deg);
			ap2fg_send.longitude_deg=hton_double(ap2fg_send.longitude_deg);
			ap2fg_send.altitude_ft=hton_double(ap2fg_send.altitude_ft);
			ap2fg_send.altitude_agl_ft=hton_double(ap2fg_send.altitude_agl_ft);
			ap2fg_send.roll_deg=hton_double(ap2fg_send.roll_deg);
			ap2fg_send.pitch_deg=hton_double(ap2fg_send.pitch_deg);
			ap2fg_send.heading_deg=hton_double(ap2fg_send.heading_deg);
#endif


			//sendto(fd_sock_send, &ap2fg_send, sizeof(ap2fg_send), 0, (struct sockaddr *)&udp_sendto_addr, sizeof(udp_sendto_addr));
			send_udp_data((unsigned char*)&ap2fg_send,sizeof(ap2fg_send));



		}

		if(main_task.maintask_cnt>=MAX_MAINTASK_TICK)
		{
			main_task.maintask_cnt=0;
		}
	}


	return 0;
}

