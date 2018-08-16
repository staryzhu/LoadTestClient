/**************************************************************
*Copyright(C),2016,CLO
*FileName:CBetKLSC.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/12
*Description:开乐三彩游戏类
**************************************************************/

#ifndef CBETKLSC_H
#define CBETKLSC_H
#include "CGameBase.h"
#include "Common.h"
#ifndef DEBUGONWIN
#include <pthread.h>
#endif

class CBetKLSC : public CGameBase
{
public:
    CBetKLSC(string strSAP, CHttpClient *httpClient) :CGameBase(strSAP, httpClient)
    {
        //initSema();
    };
    ~CBetKLSC()
    {
    };
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
    void GameOperation(string strRules);
private:
    void Transaction(string lpszUID, string strWagerIssue);//交易
    static bool isSemaInit;
    static pthread_mutex_t m_Trans_lock;
};
#endif