/**************************************************************
*Copyright(C),2016,CLO
*FileName:CCrew.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:
**************************************************************/

#include "CCrew.h"
#include <time.h>
bool CCrew::CrewInit(int size,int maxsize, unsigned long waitTime)
{
    int c = 0;
    this->m_nClientSize = size;
    this->m_nTimeDuration = maxsize;
    this->m_lTotalTaskSize = 0;
    this->m_bShutdown = false;
    this->m_threads = new vector<pthread_t>();
    this->m_tasks = new vector<CTask>();
    m_nMaxClientTime = 0;//单线 程下最大处理用时
    m_nMinClientTime = 9999999;//最小处理用时（单位毫秒）
    m_nConCountWhenMaxTime = 0;// 最耗时时连接数量
    m_nConCountWhenMinTime = 0;//最小耗时时的连接数量
    m_nTotalTransactionCount = 0;// 一共模拟的交易次数
    m_nTotalConnectionCount = 0;//所有连接次数总和
    m_nConcourrentCount = 0;// 并发量(连接数/秒)
    m_lTotalCaltime  = 0;
    m_nThreadSleep = waitTime;
    for (int i = 0; i < ERRORCODE_LENGTH; i++)
    {
        m_ResultCount[i] = 0;
    }
#ifndef DEBUGONWIN
    if ((c = pthread_mutex_init(&(this->lock), NULL)) != 0)
        return false;
    if ((c = pthread_mutex_init(&(this->result_lock), NULL)) != 0)
        return false;
    if ((c = pthread_cond_init(&(this->start), NULL)) != 0)
        return false;
    if ((c = pthread_cond_init(&(this->empty), NULL)) != 0)
        return false;
#endif
    return true;
}

bool CCrew::AddThread(pthread_t *td)
{ 
    this->m_threads->push_back(*td);
    return false; 
}

bool CCrew::AddTask(CTask *task)
{
    //total计算的是所有任务rep的总和
    this->m_lTotalTaskSize += task->GetRep();//total = sum(rep * task_size)
    this->m_tasks->push_back(*task);
    return false; 
}

//发送开始信号到所有线程，并
//等待所有运行线程，直到所有线程结束
bool CCrew::JoinAllThread()
{
    int c = 0;
    int x = 0;
    void * pload = NULL;
    //cout start Time
    time_t lt;
    lt =time(NULL);
    cout << "Start Time: " << ctime(&lt) << endl;
#ifndef DEBUGONWIN
    if ((c = pthread_mutex_lock(&(this->lock))) != 0)
    {
        //mutex lock
    }
    if ((c = pthread_cond_broadcast(&(this->start))) != 0)//broadcast cond-signal to all waiting thread
    {
    }
    if ((c = pthread_mutex_unlock(&(this->lock))) != 0)
    {
        //mutex unlock
    }

    for (x = 0; x < this->m_nClientSize; x++)
    {//wait for all pressure thread exit
        if ((c = pthread_join((*m_threads)[x], &pload)) != 0) 
        {
        }
    }
    //cout end Time
    lt =time(NULL);
    cout << "End Time: " << ctime(&lt) << endl;
#endif
    return true; 
}

//从task中获取任务。获取时，先遍历直到发现有rep值不为0的task则返回
//在时间相关的情况下，rep值为-1
CTask *CCrew::GetTask(int &nReps, long &nUID)
{
    int i = 0;
    int c = 0;
    CTask *task = NULL;

#ifndef DEBUGONWIN
    if ((c = pthread_mutex_lock(&(this->lock))) != 0)
    {
        //mutex lock
    }
#endif
    //当时间运行设置不为0时，则将第一个task返回
    if(m_nTimeDuration > 0)
    {
        task = &((*m_tasks)[0]);
    }
    else
    {
        for(i = 0; i < m_tasks->size(); i++)
        {
            if ((*m_tasks)[i].GetRep() > 0)
            {
                nReps = (*m_tasks)[i].DecRep(m_nClientSize);
                task = &((*m_tasks)[i]);
                break;
            }
        }
    }

    if(task)//此处加入task为null的验证，防止野指针
    {
        nUID = task->GetUIDAndINC1();
    }
#ifndef DEBUGONWIN
    if ((c = pthread_mutex_unlock(&(this->lock))) != 0)
    {
        //mutex unlock
    }
#endif
    return task;
}

//释放空间
bool CCrew::CrewDestory()
{
    delete m_threads;
    delete m_tasks;
}

CTask * CCrew::GetTaskByIndex(int index)
{
    CTask *task = NULL;
    if (index < (*m_tasks).size() && index >= 0)
    {
        task = &((*m_tasks)[index]);
    }
    return task;
}