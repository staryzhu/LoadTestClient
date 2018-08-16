/**************************************************************
*Copyright(C),2016,CLO
*FileName:CCrew.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:事物类,线程同步和任务管理
**************************************************************/

#ifndef CCREW_H
#define CCREW_H

#include "Common.h"
#include "CTask.h"
#include <vector>
#ifndef DEBUGONWIN
#include <pthread.h>
#endif

using namespace std;

class CCrew
{
public:
    CCrew()
    {

    };
    //初始化设置模拟客户端的数量和最大的允许范围
    CCrew(int size, int maxsize)
    {
        CrewInit(size, maxsize, 0);
    };
    
    ~CCrew()
    {
        CrewDestory();
    };
    bool AddThread(pthread_t *td);//将线程加入到监控队列
    bool AddTask(CTask *task);//将从config.ini中读取的task加入到任务队列中
    CTask *GetTask(int &nReps, long &nUID);//客户读取任务所用接口
    bool JoinAllThread();//start all waiting thread and process the task
    bool CrewInit(int size, int maxsize, unsigned long waitTime);//初始化协助类成员
    bool CrewDestory();//释放资源
    CTask *GetTaskByIndex(int index);//获取task运行结果
public:
    bool          m_bShutdown;
    int             m_nClientSize;//客户数量，并发线程数量
    int             m_nTimeDuration;
    long          m_lTotalTaskSize;//task size
    long          m_lTotalCaltime;//使用gettimeofday计算的总时间
#ifndef DEBUGONWIN  
    pthread_mutex_t  lock;//用于获取游戏任务时多线程共享资源
    pthread_cond_t   start;//concourrent start
    pthread_cond_t   empty;//task empty
    pthread_mutex_t result_lock;//填写结果时需要加锁
#endif
    //结果信息
    int m_nMaxClientTime;//单线程下最大处理用时
    int m_nMinClientTime;//最小处理用时（单位毫秒）
    int m_nConCountWhenMaxTime;// 最耗时时连接数量
    int m_nConCountWhenMinTime;//最小耗时时的连接数量
    int m_nTotalTransactionCount;// 一共模拟的交易次数
    int m_nTotalConnectionCount;//所有连接次数总和
    int m_nConcourrentCount;// 并发量(连接数/秒)
    int m_ResultCount[ERRORCODE_LENGTH];
    unsigned long m_nThreadSleep;//客户端间断时间
    /*
    {
        { NOTHING, 0 },
        { SUCCESS, 0 },
        { CONNECT_ERROR, 0 },
        { JSON_ERROR, 0 },
        { RETURN_CODE_ERROR, 0 },
    };*/ //游戏返回状态次数
private:
    vector<CTask>    *m_tasks;//存放任务
    vector<pthread_t> *m_threads;//spilit thread and task,make concurrent
};

#endif /*CCREW_H*/