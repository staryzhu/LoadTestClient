/**************************************************************
*Copyright(C),2016,CLO
*FileName:CClient.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:客户类函数的具体实现文件
**************************************************************/
#include "CClient.h"
#include "CTimer.h"
//#include <stdlib.h>
using namespace std;

CClient::~CClient()
{
    ReleaseConnect();
}
CClient::CClient(CCrew *crew)
{
    m_pcrew = crew;
    m_work.crew = m_pcrew;
    m_work.httpClient = NULL;
    m_pHttpClient = NULL;
    //CreateThread();
    //m_pcrew->AddThread(&m_handler);
}

bool CClient::CreateThread()
{
    int c = 0;
//    cout << "Create Thread" << endl;
#ifndef DEBUGONWIN
    //创建客户线程
    if ((c = pthread_create(&m_handler, NULL, ClientThread, (void *)&m_work)) != 0)
    {
    /*
        switch (errno) 
        {
            case EINVAL: { fprintf(ERROR, "Error creating additional threads %s:%d", __FILE__, __LINE__);  break; }
            case EPERM:  { fprintf(ERROR, "Inadequate permission to create pool %s:%d", __FILE__, __LINE__);  break; }
            case EAGAIN: { fprintf(ERROR, "Inadequate resources to create pool %s:%d", __FILE__, __LINE__);  break; }
            case ENOMEM: { fprintf(ERROR, "Exceeded thread limit for this system %s:%d", __FILE__, __LINE__);  break; }
            default:     { fprintf(ERROR, "Unknown error building thread pool %s:%d", __FILE__, __LINE__);  break; }
        } return false;
    */
    return false;
    }
#endif
    return true;
}

void *CClient::ClientThread(void *work)
{
    int c = 0;
    int reps = 0;
    int index = 0;
    long uid = 0;
    CCrew *pcrew = (CCrew *)(((WORK *)work)->crew);
    void *httpClient = ((WORK *)work)->httpClient;//将work中的http句柄传入到客户线程
    CTask *task;
    CTimer time(NULL);
    RESULTINFO rsltInfo;
    int nRand = pthread_self();
    nRand = nRand > 0 ? nRand:0 - nRand;
    //cerr << "thread id :" << nRand << endl;
    if (httpClient == NULL)
    {
        cerr << "Connection is not attach to client" << endl;
        return (void *)0;
    }
#ifndef DEBUGONWIN
    if (!pcrew->m_bShutdown && !global_shutdown)
    {//客户线程阻塞等待start广播信号被释放
        if ((c = pthread_mutex_lock(&(pcrew->lock))) != 0)
        {
            //mutex lock
        }
        //此处应该加入任务是否被添加完毕的判断量，因为start信号可能会被意外唤醒
        if ((c = pthread_cond_wait(&(pcrew->start), &(pcrew->lock))) != 0)//hold on there,wait for start cond-signal
        //pthread wait
        {
            cerr << "Client Thread Wait start Exception" << endl;
            pthread_exit(NULL);
        }
        if ((c = pthread_mutex_unlock(&(pcrew->lock))) != 0)
        {
            //mutex unlock
        }
    }else
    {
        cerr << "Client Thread Not Run Exception" << endl;
        pthread_exit(NULL);
    }
    //cout << "Start Running" << endl;
    //srandom(random());//多线程下，制作随机种子，防止random数相同
#endif
    do
    {
        rsltInfo.bIsSuccess        = false;//初始化结果内容
        rsltInfo.nConnectionCount  = 0;
        rsltInfo.nTransactionCount = 0;
        rsltInfo.errorInfo         = NOTHING;
        //获取游戏内容
        task = pcrew->GetTask(reps, uid);//;get task in critical section
#ifndef DEBUGONWIN
        if (pcrew->m_bShutdown == true || global_shutdown == 1)//如果shutdown被置为true则退出所有客户线程
        {
            pthread_exit(NULL);
            break;
        }

        if (task == NULL || pcrew->m_lTotalTaskSize <= 0)//当任务池子为空时，发送池子为空的信号，并退出客户线程
        {//如果游戏内容数量已经为0，则广播此消息到所有客户线程，并退出线程
            //此消息同意会被时间侦测线程监听，并记录时间
            if ((c = pthread_mutex_lock(&(pcrew->lock))) != 0)
            {
                //mutex lock
            }
            pcrew->m_bShutdown = true;
            if ((c = pthread_cond_broadcast(&(pcrew->empty))) != 0) //send cond-empty signal to timer and watcher thread to stop all
            {

            }

            if ((c = pthread_mutex_unlock(&(pcrew->lock))) != 0)
            {
                //mutex unlock
            }
            pthread_exit(NULL);//退出线程
            break;
        }
        
#endif
        if(index > 9998 || index < 1000)
        {
            index = 1000;
        }
        //计时开始
        time.TimerStart();
        //开始运行游戏
        task->Run(reps, httpClient, rsltInfo, nRand, index ++, uid);
        time.TimerStop();
        //计算经过的时间
        int timeElapse = time.TimerElapse() / 1000;
        //写入结果
        if ((c = pthread_mutex_lock(&(pcrew->result_lock))) != 0)
        {
            //mutex lock
        }
        pcrew->m_lTotalCaltime += timeElapse;
        //current task statics当前任务的数据统计
        if (timeElapse > task->m_nMaxClientTime && rsltInfo.bIsSuccess)
        {
            task->m_nMaxClientTime = timeElapse;//选出最大时间
            task->m_nConCountWhenMaxTime = rsltInfo.nConnectionCount;
        }
        if (timeElapse < task->m_nMinClientTime && rsltInfo.bIsSuccess)
        {
            task->m_nMinClientTime = timeElapse;//选出最小时间
            task->m_nConCountWhenMinTime = rsltInfo.nConnectionCount;
        }
        task->m_ResultCount[rsltInfo.errorInfo]++;//游戏当前结果的状态计数自增1
        task->m_nTotalConnectionCount += rsltInfo.nConnectionCount;
        task->m_nTotalTransactionCount += rsltInfo.nTransactionCount;

        //all task statics全局任务数据统计
        if (timeElapse > pcrew->m_nMaxClientTime && rsltInfo.bIsSuccess)
        {
            pcrew->m_nMaxClientTime = timeElapse;//选出最大时间
            pcrew->m_nConCountWhenMaxTime = rsltInfo.nConnectionCount;
        }
        if (timeElapse < pcrew->m_nMinClientTime && rsltInfo.bIsSuccess)
        {
            pcrew->m_nMinClientTime = timeElapse;//选出最小时间
            pcrew->m_nConCountWhenMinTime = rsltInfo.nConnectionCount;
        }
        pcrew->m_ResultCount[rsltInfo.errorInfo]++;//游戏当前结果的状态计数自增1
        pcrew->m_nTotalConnectionCount += rsltInfo.nConnectionCount;
        pcrew->m_nTotalTransactionCount += rsltInfo.nTransactionCount;

        if ((c = pthread_mutex_unlock(&(pcrew->result_lock))) != 0)
        {
            //mutex unlock
        }
    } while (true);
    //cout << "ClientThread Exit" << endl;
    return NULL;
}

void CClient::AttachConnect(CHttpClient *httpClient)
{
    if (httpClient != NULL)
    {
        m_pHttpClient = httpClient;
    }else
    {
        m_pHttpClient = new CHttpClient("HTTP", "");//初始化Http连接对象,SAP传入空值
        if (!m_pHttpClient->Init())
        {
            cout << "http client init failed" << endl;
        }
    }
    m_work.httpClient = (void *)m_pHttpClient;
}

void CClient::ReleaseConnect()
{
    if (m_pHttpClient)
    {
        delete m_pHttpClient;
    }
}

