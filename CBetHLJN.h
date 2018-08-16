/**************************************************************
*Copyright(C),2016,CLO
*FileName:CBetHLJN.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/1
*Description:欢乐基诺游戏类
**************************************************************/

#ifndef CBETHLJN_H
#define CBETHLJN_H
#include "CGameBase.h"
#include "Common.h"
#ifndef DEBUGONWIN
#include <pthread.h>
#endif

class CBetHLJN : public CGameBase
{
public:
    CBetHLJN(string strSAP, CHttpClient *httpClient) :CGameBase(strSAP, httpClient)
    {
        //initSema();
    };
    ~CBetHLJN(){};
    void GameOperation(string strRules);//欢乐基诺游戏操作函数
    static bool initSema()//初始化信号量
    {
        if(isSemaInit == false)
        {
            if ((pthread_mutex_init(&(m_Trans_lock), NULL)) != 0)
                return false;
        }
        isSemaInit = true;
        return true;
    };
private:
    void Transaction(string lpszUID, string strWagerIssue);//交易
    static bool isSemaInit;
    static pthread_mutex_t m_Trans_lock;
};
#endif