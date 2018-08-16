/**************************************************************
  *Copyright(C),2016,CLO
  *FileName:Main.cpp
  *Author:Stary(ZhiGuang) Zhu
  *Version:1.0
  *Date:2016/4/1
  *Description:
**************************************************************/
#include <signal.h>
#include <unistd.h>
#include "CFlowBase.h"
#include "CSysConf.h" 
//#include "ILLog.h"
using namespace ServiceFramework_NS;
//global_shutdown变量控制程序是否继续运行，如果设置为1则程序退出，并显示结果
int global_shutdown = 0;
#define CONFIGFILE "./config.ini"

void sig_opt(int sig)
{
    cerr << "Recieve Signal :" << sig << endl;
    global_shutdown = 1;
}

//初始化信号接收器，将传入的Ctrl-c信号进行处理后退出
void init_sig()
{
    struct sigaction act;
    act.sa_handler = sig_opt;
    
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);
    
    act.sa_flags = 0;

    sigaction(SIGINT, &act, 0);
    sigaction(SIGTERM, &act, 0);
}

int main(int argc,char *argv[])
{
    init_sig();
    // if (!CSysConf::Instance()->init(CONFIGFILE))
    // {
        // cout << "load config file failed" << endl;
    // }

    // if (!ILLog::Instance()->Init())
    // {
        // cout << "init log instance failed " << endl;
    // }

    CDisposer disposer;

    if (disposer.ParseArgs(argc, argv))
    {
        disposer.MainFlow();//程序入口
    }
}