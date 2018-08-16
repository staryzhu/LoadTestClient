/**************************************************************
*Copyright(C),2016,CLO
*FileName:CBetTRANS.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/1
*Description:普通交易类
此游戏类为单例模式，节省重复创建此类代来的时间消耗
和内存外部碎片

测试后无法进行单例模式，犹豫多线程下critical区域涉及
保护问题
**************************************************************/

#ifndef CBETTRANS_H
#define CBETTRANS_H
#include "CGameBase.h"
#include "Common.h"

class CBetTRANS : public CGameBase
{
public:
    CBetTRANS(string strSAP, CHttpClient *httpClient) :CGameBase(strSAP, httpClient)
    {

    };
    void GameOperation(string strRules)
    { 
        //Request(strRules);
        cout << "Transaction" << endl; 
    };
private:
   // static CBetTRANS *pTransaction;
};
#endif