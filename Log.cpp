#include "Log.h"

CLog* CLog::m_Log = NULL;
CLog* CLog::Instance()
{
  if (m_Log == NULL)
    m_Log = new CLog();
  return m_Log;
}

void CLog::removeInstance()
{
    if (m_Log != NULL)
    {
        delete m_Log;
        m_Log = NULL;
    }
}


int CLog::Init(const char * filepath)
{
    m_pfLogFile = fopen(filepath, "at+");
    if(NULL == m_pfLogFile)
    {
        return -1;
    }
    return 0;
}


CLog::CLog()
{
    m_pfLogFile = NULL;
    //memset(m_cInfo,NULL,sizeof(m_cInfo));
}

CLog::~CLog()
{
    if (NULL != m_pfLogFile)
    {
        fclose(m_pfLogFile);
        m_pfLogFile = NULL;
    }
}

int CLog::WriteLogInfo(const char *pInfo)
{
    if(NULL != m_pfLogFile)
    {
        fprintf(m_pfLogFile,"%s\n",pInfo);
        fflush(m_pfLogFile);
        return 0;
    }
    return -1;
}

int CLog::WriteErrInfo(const char *pInfo)
{
    if(NULL != m_pfLogFile)
    {
        fprintf(m_pfLogFile,"%s\n",pInfo);
        fflush(m_pfLogFile);
        return 0;
    }
    return -1;
}

#ifdef TEST
int main()
{
    int num = 99;
    CLog::Instance()->Init("./Log2017.log");
    CLog::Instance()->WriteLogInfo("This is a test,Num");
    CLog::removeInstance();
    return 0;
}
#endif