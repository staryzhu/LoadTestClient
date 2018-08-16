/**************************************************************
*Copyright(C),2016,CLO
*FileName:CGameBase.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:游戏基类，所有游戏的添加需继承此基类
需将每个派生出来的游戏类设置为单例模式

设计临界区问题，单例模式无法满足
**************************************************************/

#ifndef CGameBase_H
#define CGameBase_H
#include <string>
#include "Common.h"
#ifndef DEBUGONWIN
#include "HttpClient.h"
using namespace ServiceFramework_NS;
#endif
using namespace std;


class CGameBase
{
public:
    CGameBase(string strSAP, CHttpClient *httpClient);//SAP,绑定http句柄
    virtual ~CGameBase();
    string Request(string strURL);//发送http请求,输入访问的URL地址
    virtual void GameOperation(string strRules){ cout << "CGameBase Do Nothing" << endl; };//游戏操作函数，由子类函数具体实现游戏操作
    void SetURL(string strURL);//动态设置URL
private:
//    static bool isHttpInit;
#ifndef DEBUGONWIN
    CHttpClient *m_pHttpClient;//http连接
#endif
protected:
    string m_strURL;
    bool    m_bIsBet;//判断是否进行模拟交易，如果是true则进行模拟交易，可能会发生多次http连接。如果是false只发生一次http连接
public:
    int m_nUID;//user ID
    int m_nTransactionCnt;
    int m_nConnectionCnt;
    bool m_bSuccess;//
    ERRORCODE m_error;
    METHOD m_method;
    int m_nRandom;//用作多线程下random数值一样的问题
    int m_nIndex;//增加随机唯一性
    string m_strWagerIssue;
};

#endif