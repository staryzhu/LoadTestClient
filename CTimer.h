/**************************************************************
*Copyright(C),2016,CLO
*FileName:CTimer.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/1
*Description:时间控制类，计算游戏时间，和定时
**************************************************************/
#ifndef CTIMER_H
#define CTIMER_H
#include "CCrew.h"
#ifndef DEBUGONWIN
#include<sys/time.h>
#endif

class CTimer
{
public:

    CTimer(CCrew *crew)//传入协作类，主要传入监听信号和互斥锁
    {//当传入crew为null时，不适用线程计时
        this->pcrew = crew;
    };

    CTimer()
    {
        this->pcrew = NULL;//默认构造函数，设置pcrew为null,不使用线程计时
    };

    ~CTimer(){};

    void TimerStart()//计时器开始
    {
#ifndef DEBUGONWIN
        gettimeofday(&start,NULL);
#endif
    };

    void TimerStop()//计时器结束
    {
#ifndef DEBUGONWIN
        gettimeofday(&end, NULL);
#endif
    };

    void AttachTimer(int duration);//创建时间线程,参数为0时，默认使用timer线程,如果pcrew为null时则调用TimerStart

    long TimerElapse();//获取时间段结果(end_time - start_time)
    
    void WaitForExit();//等待时间线程退出

private:
    static void * TimerThread(void *);//时间测算线程
    static void * DurationThread(void *);//当duration设置不为0时，则进行时间控制
public:
    long m_lTimeDuration;
    pthread_t m_handler;
private:
    CCrew *pcrew;
#ifndef DEBUGONWIN
    struct timeval start;
    struct timeval end;
    struct timeval result;
#endif
};

#endif