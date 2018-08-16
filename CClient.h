/**************************************************************
*Copyright(C),2016,CLO
*FileName:CClient.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:模拟客户类,为每个模拟客户创建一个线程
**************************************************************/
#ifndef CCLIENT_H
#define CCLIENT_H

#include "CBaseThread.h"
#include "CCrew.h"

//WORK结构体，向客户线程传递工作
typedef struct {
    CCrew *crew;//事物
    void *httpClient;//http句柄
}WORK;

class CClient : public CBaseThread
{
public:
    CClient(CCrew *crew);
    ~CClient(); 
    bool CreateThread();
    bool SyncThread(){};
    //此处如此添加的原因：
    //1.保证实际并发量，必须保证连接数与模拟客户数量一致或大于
    //2.无法在线程中动态创建http句柄，因为curl_globe_init线程不安全
    void AttachConnect(CHttpClient *httpClient);//在运行线程前，需传入http连接句柄,传入NULL，则自动创建
    void ReleaseConnect();//释放http连接资源
private:
    static void *ClientThread(void *work);
#ifndef DEBUGONWIN
    CHttpClient *m_pHttpClient;//http连接
#endif
    WORK m_work;
public:
    pthread_t m_handler;
};

#endif