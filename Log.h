/*
*   写日志类
*/
#ifndef __header_of_LOG__
#define __header_of_LOG__


#include <iostream>
#include <stdio.h>
using namespace std;
 
class CLog
{
private:
    CLog();
    static CLog* m_Log;
public:
    ~CLog();
    static CLog* Instance();
    static void removeInstance();
    int Init(const char * filepath);

public:
    //日志文件
    FILE* m_pfLogFile;
    char m_cInfo[255];
    int WriteLogInfo(const char *pInfo);
    int WriteErrInfo(const char *pInfo);
};

#endif