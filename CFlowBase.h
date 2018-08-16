/**************************************************************
*Copyright(C),2016,CLO
*FileName:CFlowBase.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:流程服务类，负责整体的调度，读取用户配置
创建客户线程和添加任务
**************************************************************/
#ifndef CFLOWBASE_H
#define CFLOWBASE_H

#include "Common.h"
#include "CCrew.h"
#include "CClient.h"


class CFlowBase
{
public:
    CFlowBase();
    ~CFlowBase();
};

class CDisposer : public CFlowBase
{
public:
    CDisposer();
    ~CDisposer();
    void MainFlow();//主要流程函数
    bool ParseArgs(int argc, char *argv[]);//分析输入参数
private:
    long ReadConfig(const char *strItem);//从ini文件中读取crew参数,并返回integer值
    bool ReadTaskInfo(CTask *task,int seq);//从ini文件中读取task参数,seq为第几个task
    void PrintHelp();
    void OutPutStatics(CCrew &crew);//将crew中收集的数据输出
public:
    string m_ConfigPath;
    unsigned long m_nClientDelay;//模拟客户端间断访问时间
private:
    int m_nClientSize;
    CClient *(m_ClientArray[MAX_CONNECTION]);//CClient指针数组
};


#endif