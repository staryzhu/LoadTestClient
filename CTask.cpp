/**************************************************************
*Copyright(C),2016,CLO
*FileName:CTask.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/05/03
*Description:Task操作类实现
**************************************************************/
#include "CGameBase.h"
#include "CTask.h"
#include "CBetHLJN.h"
#include "CBetKLSC.h"
#include "CBetTRANS.h"

void CTask::Run(int nReps, void *httpClient, RESULTINFO & resultInfo, int randBase, int index, long uid)
{
    CGameBase *pGame = NULL;
    resultInfo.errorInfo = NOTHING;
    pGame = GetGame((CHttpClient *)httpClient);
    for (int i = 0; i < 1; i++)//此处加入循环，可减轻创建和回收game对象的消耗
    {
        if(pGame)
        {
            pGame->m_nUID    = uid;//设置uid
            pGame->m_nRandom = randBase;
            pGame->m_nIndex  = index;
            pGame->m_strWagerIssue = m_strWagerIssue;
            pGame->m_method = this->m_method;//将task的method赋值给具体游戏
            pGame->GameOperation(m_rules);//动态挂载具体游戏内容
            resultInfo.bIsSuccess        = pGame->m_bSuccess;
            resultInfo.nConnectionCount  = pGame->m_nConnectionCnt;
            resultInfo.nTransactionCount = pGame->m_nTransactionCnt;
            resultInfo.errorInfo         = pGame->m_error;
        }
    }
    if(pGame)
    {
        delete pGame;//释放资源
    }
}

CGameBase *CTask::GetGame(CHttpClient *httpClient)
{
    CGameBase *pGB = NULL;
    //cout << "Game:" << tGame[m_gtype].GName << endl;
    //根据从config.ini中读取的游戏种类，挂载游戏对象
    switch (m_gtype)
    {//将游戏对象的创建过程移到task创建过程,这样可以节省创建时间,但是如果访问量大的话，对内存开销较大
        //理想方案，由mgr线程创建与client相等的每种类型的游戏，由环形单链表管理（类似缓冲区）
        case GAME_TRANSACTION:
            pGB = new CBetTRANS(m_url, httpClient);//CBetTRANS::getInstance(m_url);
            break;
        case GAME_HLJN:
            pGB = new CBetHLJN(m_url, httpClient);
            break;
        case GAME_KLSC:
            pGB = new CBetKLSC(m_url, httpClient);
            break;
        default:
            cout << "Game:NULL" << endl;
            pGB = new CGameBase(m_url, httpClient);
            break;
    }
    return pGB;
}
