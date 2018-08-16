/**************************************************************
*Copyright(C),2016,CLO
*FileName:CGame.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:此cpp中实现了CGameBase类和CTask类
**************************************************************/
#include "CGameBase.h"
//#include "CTask.h"
//#include "CBetHLJN.h"
//#include "CBetKLSC.h"
//#include "CBetTRANS.h"

CGameBase::CGameBase(string strSAP, CHttpClient *httpClient)
{
    m_strURL = strSAP;
    m_pHttpClient = httpClient;
    m_nTransactionCnt = 0;
    m_nConnectionCnt = 0;
    m_bSuccess = false;//
    m_error = NOTHING;
    m_bIsBet = false;
    m_method = RANDOM;
}

CGameBase::~CGameBase()
{
//    DestoryHttp();// httpclient实例由外部创建和释放，防止多线程下临界区
}

void CGameBase::SetURL(string strURL)
{
    m_strURL = strURL;
}

string CGameBase::Request(string strURL)
{//发送http请求到服务器,外包call函数，为以后可以拓展其它接口留有余地
    string strResponse = "";
#ifndef DEBUGONWIN
    if (m_pHttpClient == NULL)
    {
        return "";
    }
    if (!m_pHttpClient->Call(strURL, strResponse))
    {
        //cout << "Send Request Failed" << endl;
        return strResponse;
    }
//    cout << "Http Response:" << strResponse << endl;
    return strResponse;
#endif
}

