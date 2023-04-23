//2、跟着最近的东西的小车
//始终朝着离自己最近的东西前进，舵机控制超声波模块左右摇摆，选取距离最近的角度，调整左右各2个轮子角度并前进，其中，调参可以自动进行

#include<Servo.h>
#include "Microduino_MotorPlus.h"
#include <Microduino_Ultrasonic.h>

Ultrasonic Ultrasonic1(ULTRASONIC_ADDR_1);//将超声波实例化
uint16_t Distance;

int turntime = 100;
int beta = 100;
int dis_min =1000;//初始值1000mm，即1m,即最大测距距离
int sita1 = 0;
int sita2 = 0;

#define Ser_pin 4 //舵机
Servo servo1;//舵机
MotorPlus motor(MOTOR_ADDR4);

void setup()
{
  pinMode(Ser_pin,OUTPUT);//舵机
  servo1.attach(Ser_pin);//初始化舵机，并设置引脚4
  Serial.begin(9600);
  Ultrasonic1.begin();//初始化
  delay(2000);
}

void loop()
{
  int min = 1000;//初始化
  int sita1 = 0;
  int sita2 = 0;
  int i =0;
  for(i=1;i<179;i+=2)//从左到右扫描，总共扫描90次
  {
    servo1.write(i);
    Distance = Ultrasonic1.getDistance();//单位：mm
    Serial.println(Distance);
    //读取此时超声波的值，转化为dis
    if(Distance<dis_min)
    {
      sita1 = i;//记录最小dis对应i
      dis_min = Distance;
    }
    delay(20);//1.8秒完成一次扫描
  }
  motor.setSpeed(sita1-90, 90-sita1);  //同时设置电机1和电机2速度
  delay(turntime);
  motor.setSpeed(FREE, FREE);  //同时设置电机1和电机2为释放状态，即速度为0
  
  //以下是负反馈过程
  for(i=179;i>1;i-=2)//从左到右扫描，总共扫描90次
  {
    servo1.write(i);
    Distance = Ultrasonic1.getDistance();//单位：mm
    Serial.println(Distance);
    //读取此时超声波的值，转化为dis
    if(Distance<dis_min)
    {
      sita2 = i;//记录最小dis对应i,如果前面给转过头了，这会就可以调整
      dis_min = Distance;
    }
    delay(20);//1.8秒完成一次扫描
  }
  turntime = turntime - (90-sita2)/(sita1-90);//短的time=长的time－多转的角度/角速度。我试了一下这个式子在各个情况下都是成立的
  motor.setSpeed(sita2-90, 90-sita2);  //同时设置电机1和电机
  delay(turntime);
  motor.setSpeed(FREE, FREE);  //同时设置电机1和电机2为释放状态，即速度为0
  //负反馈结束，效果为修正time值并转到正确角度
  //以下为前进
  motor.setSpeed(100, 100);  //同时设置电机1和电机2为释放状态，即速度为0
  delay(Distance*beta);//根据距离换算前进时间
}

