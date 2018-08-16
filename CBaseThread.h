/**************************************************************
*Copyright(C),2016,CLO
*FileName:CBasethread.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:
**************************************************************/

#ifndef CBASETHREAD_H
#define CBASETHREAD_H 

#include "CCrew.h"

class CBaseThread
{
public:
    CBaseThread(){};
    ~CBaseThread(){};
    void GenThread(){};
public:
    CCrew *m_pcrew;
};


#endif /*CBASETHREAD_H*/