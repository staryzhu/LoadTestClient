/**************************************************************
*Copyright(C),2016,CLO
*FileName:CTimer.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/7
*Description:时间控制由两种模式控制
流程控制
信号控制
**************************************************************/

#include "CTimer.h"

void CTimer::AttachTimer(int duration)
{
    int c = 0;
    m_lTimeDuration = duration;
    m_handler = 0;
    if (pcrew == NULL)
    {
        TimerStart();
        return;
    }
#ifndef DEBUGONWIN
    if(duration != 0)
    {
        //当duration不为0时，设置整个运行环境为时间控制，而不是task自身的次数限制
        //并且只运行第一个task
        if ((c = pthread_create(&m_handler, NULL, DurationThread, (void *)this)) != 0)
        {
            cerr <<"Create Duration Thread Exception" << endl;
        }
        return;
    }
    //创建客户线程
    if ((c = pthread_create(&m_handler, NULL, TimerThread, (void *)this)) != 0)
    {
        cerr <<"Create Timer Thread Exception" << endl;
    }
#endif
}

void CTimer::WaitForExit()
{
    int c = 0;
    void * pload = NULL;
    if (m_handler != 0)
    {
        if ((c = pthread_join(m_handler, &pload)) != 0) 
        {
        }
    }else
    {
        cerr << "Thread Handler not be initialized" << endl;
    }
}

void * CTimer::DurationThread(void *arg)
{
    int c = 0;
    CTimer *ptimer = (CTimer *)arg;
    long dur_time = ptimer->m_lTimeDuration;
    if (!ptimer->pcrew->m_bShutdown)
    {
        if ((c = pthread_mutex_lock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex lock
        }
        //时间线程阻塞等待start信号广播
        if ((c = pthread_cond_wait(&(ptimer->pcrew->start), &(ptimer->pcrew->lock))) != 0)//hold on there,wait for start cond-signal
            //pthread wait
        {
            cerr << "Timer Wait for Start Error" << endl;
        }
        if ((c = pthread_mutex_unlock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex unlock
        }
        ptimer->TimerStart();//计时器开始
    }
    else
    {
        pthread_exit(NULL);
    }
    
    //此处开始进入时间的循环中，知道时间停止则退出，并发送empty信号到所有线程
    for(int i = 0; i < dur_time && !global_shutdown; i++)
    {
        sleep(1);
    }
    //发送empty信号到所有线程
    if ((c = pthread_mutex_lock(&(ptimer->pcrew->lock))) != 0)
    {
        //mutex lock
    }
    ptimer->pcrew->m_bShutdown = true;
    if ((c = pthread_cond_broadcast(&(ptimer->pcrew->empty))) != 0) //send cond-empty signal to timer and watcher thread to stop all
    {

    }
    if ((c = pthread_mutex_unlock(&(ptimer->pcrew->lock))) != 0)
    {
        //mutex unlock
    }
    ptimer->TimerStop();//计时器结束
    cout << "Duration Exit" << endl;
}

void * CTimer::TimerThread(void *arg)//将时间对象this传入时间线程
{
    int c = 0;
    struct timeval now;
    struct timespec settime;
    CTimer *ptimer = (CTimer *)arg;
#ifndef DEBUGONWIN
    if (!ptimer->pcrew->m_bShutdown)
    {
        if ((c = pthread_mutex_lock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex lock
        }
        //时间线程阻塞等待start信号广播
        if ((c = pthread_cond_wait(&(ptimer->pcrew->start), &(ptimer->pcrew->lock))) != 0)//hold on there,wait for start cond-signal
            //pthread wait
        {
            cerr << "Timer Wait for Start Error" << endl;
        }
        if ((c = pthread_mutex_unlock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex unlock
        }
        ptimer->TimerStart();//计时器开始
        cout << "Timer Clock Start Counting" << endl;
    }
    else
    {
        pthread_exit(NULL);
    }
    while(ptimer->pcrew->m_bShutdown == false && global_shutdown == 0)
    {
        //在监听到开始信号start后，启动定时器，并且开始监听任务empty信号
        if ((c = pthread_mutex_lock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex lock
        }
        gettimeofday(&now, NULL);
        settime.tv_sec = now.tv_sec + 1;
        settime.tv_nsec = now.tv_usec * 1000;
        // pthread_cond_timedwait(&cond, &mutex, &settime);
        if ((c = pthread_cond_timedwait(&(ptimer->pcrew->empty), &(ptimer->pcrew->lock), &settime)) != 0)//hold on there,wait for start cond-signal
            //pthread wait
        {
            //cerr << "Timer Wait for Task Empty Error" << endl;
        }
        if ((c = pthread_mutex_unlock(&(ptimer->pcrew->lock))) != 0)
        {
            //mutex unlock
        }
    }
    ptimer->TimerStop();//计时器结束
    cout << "Timer Watcher Exit" << endl;
#endif
}

long CTimer::TimerElapse()
{
    long usec = 0;
#ifndef DEBUGONWIN
    struct timeval result;
    //计算经过的时间
    result.tv_sec = end.tv_sec - start.tv_sec;
    result.tv_usec = end.tv_usec - start.tv_usec;
    usec = result.tv_usec;
    while (result.tv_sec > 0)
    {//将时间结果换算成微秒
        result.tv_sec --;
        usec += 1000000;
    }
#endif
    return usec;
}