/**************************************************************
*Copyright(C),2016,CLO
*FileName:CTask.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:任务类，添加游戏后，需在此类中注册游戏
**************************************************************/

#ifndef CTASK_H
#define CTASK_H
#include "CGameBase.h"
#include "Common.h"
#include "clo-rand.h"

#ifdef GENGAME
#include "CGameHLJN.h"
#include "CGameKLSC.h"
#include "CCommonTransaction.h"
#endif



class CTask
{
public:
    CTask()
    {
        m_rep = 0;
        m_bNoRep = false;
        m_nMaxClientTime = 0;//单线 程下最大处理用时
        m_nMinClientTime = 9999999;//最小处理用时（单位毫秒）
        m_nConCountWhenMaxTime = 0;// 最耗时时连接数量
        m_nConCountWhenMinTime = 0;//最小耗时时的连接数量
        m_nTotalTransactionCount = 0;// 一共模拟的交易次数
        m_nTotalConnectionCount = 0;//所有连接次数总和
        for (int i = 0; i < ERRORCODE_LENGTH; i++)
        {
            m_ResultCount[i] = 0;
        }
    };

    CTask(bool endless)
    {
        m_bNoRep = endless;
        m_rep = 0;
        m_nMaxClientTime = 0;//单线 程下最大处理用时
        m_nMinClientTime = 9999999;//最小处理用时（单位毫秒）
        m_nConCountWhenMaxTime = 0;// 最耗时时连接数量
        m_nConCountWhenMinTime = 0;//最小耗时时的连接数量
        m_nTotalTransactionCount = 0;// 一共模拟的交易次数
        m_nTotalConnectionCount = 0;//所有连接次数总和
        for (int i = 0; i < ERRORCODE_LENGTH; i++)
        {
            m_ResultCount[i] = 0;
        }
    };

    ~CTask(){};

    void SetRules(string args)//配置任务参数
    {
        this->m_rules = args;
    };

    void SetRules(string args,int nRep)//配置任务参数
    {
        this->m_rules = args;
        this->m_rep = nRep;
    };

    void SetGameType(GAMETYPE gt)//设置任务种类
    {
        m_gtype = gt;
//        cout << "Set Game Type:" << tGame[m_gtype].GName << endl;
    };

    void SetRandGameType()
    {//设置随机任务
        m_gtype = GAMETYPE(randEx() % GAME_MAX_LENGTH);
//        cout << "Set Random Game Type:" << tGame[m_gtype].GName << endl;
    };
    
    GAMETYPE GetGameType() const
    {
        return m_gtype;//获取游戏类型
    }

    int GetRep()
    {
        if (m_rep < 0)//当前不需要支持无限次
            return 0;
        return m_rep;
    };

    //将task自动递减，递减数量有用户并发量和单用户下循环次数决定
    int DecRep(int cli)
    {//不支持无限次循环
        if (m_rep == 0)//当m_rep为零则返回零
            return 0;
        //int rp = m_rep / cli + 1;
        m_rep -= 1;
        return 1;
    }

    void SetUID(int id)
    {
        m_uid = id;
    }
    
    long GetUIDAndINC1()
    {
        return m_uid ++;
    }

    void SetURL(string url)
    {
        m_url = url;
    }
    //运行任务,默认只运行一次，但有些条件下可以将重复运行次数调高（从而解决反复创建game的消耗）
    //nReps为运行次数，httpClient为访问句柄
    //randBase , index 增加随机性
    //
    void Run(int nReps, void *httpClient, RESULTINFO & rsltInfo, int randBase, int index, long uid);
private:
    CGameBase *GetGame(CHttpClient *httpClient);
private:
    int      m_uid;
    int      m_rep;//任务的重复次数-1:endless,0:stop,>0:loop times
    string m_rules;//游戏类的规则
    string m_url;//指向服务器的连接
    GAMETYPE m_gtype;//游戏类型
    CGameBase *m_pGame;//暂时保留，指向游戏的父类指针

public:
    string m_strWagerIssue;
    METHOD m_method;//task操作方式
    bool m_bNoRep;//保留，当压力测试与时间（而非数量）有关时，则置为true
    //结果信息
    int m_nMaxClientTime;//单线程下最大处理用时
    int m_nMinClientTime;//最小处理用时（单位毫秒）
    int m_nConCountWhenMaxTime;// 最耗时时连接数量
    int m_nConCountWhenMinTime;//最小耗时时的连接数量
    int m_nTotalTransactionCount;// 一共模拟的交易次数
    int m_nTotalConnectionCount;//所有连接次数总和
    int m_ResultCount[ERRORCODE_LENGTH];
    /*
    {
        { NOTHING, 0 },
        { SUCCESS, 0 },
        { CONNECT_ERROR, 0 },
        { JSON_ERROR, 0 },
        { RETURN_CODE_ERROR, 0 },
    };
    */
};

#endif
