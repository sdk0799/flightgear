/*
 * global.h
 *
 *  Created on: 2017-8-9
 *      Author: wangbo
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#define QUAD_MOTORS 4


typedef struct tagGLOBAL
{
	int cnt;

}T_GLOBAL;

typedef struct tagAP2FG{
  int throttle0;//[0..1], 0-3为四个电机的控制量
  int throttle1;
  int throttle2;
  int throttle3;
  int latitude_deg;//[deg],飞行器当前纬度坐标
  int longitude_deg;//[deg],飞行器当前经度坐标
  int altitude_ft;//[ft],飞行器当前飞行高度
  int altitude_agl_ft;//[ft],
  int roll_deg;//[deg]滚转角
  int pitch_deg;//[deg]俯仰角
  int heading_deg;//[deg]机头朝向
}T_AP2FG;

typedef struct tagFG2AP{
  int pitch_deg;//地面海拔高度，用于计算飞行器是否会撞上山
  int latitude_deg;//机场的纬度坐标，只在初始化时使用一次
  int longitude_deg;//机场的经度坐标，只在初始化时使用一次
  int heading_deg;//机头初始朝向
}T_FG2AP;


extern T_GLOBAL  gblState;
extern T_AP2FG  ap2fg;
extern T_FG2AP fg2ap;
extern T_AP2FG  ap2fg_send;



extern int latitude;
extern int longitude;
extern int altitude;



#endif /* GLOBAL_H_ */
