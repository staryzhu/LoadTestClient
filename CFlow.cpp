/**************************************************************
*Copyright(C),2016,CLO
*FileName:CFlow.cpp
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:流程类的实现，读取config信息和配置客户
线程信息和任务信息
**************************************************************/
#include "CFlowBase.h"
#include "CTimer.h"
#include <time.h>
#ifndef DEBUGONWIN
#include "ILTools.h"
#include "clo-rand.h"
#endif

const char *version_string = "1.0.0";
const char *program_name = "StressTest";
const char *author_name = "Stary(ZhiGuang) Zhu";
const char *email_address = "zhuzg@clo.com.cn";
const char *years = "2015-2016";
const char *copyright = "Copyright (C) 2016 CLO by Stary Zhu";

bool ReadValue(const char * file, const char * section, const char * key, char * outbuf, int outbufLen)
{
  char buf[512];
  char *first_ptr = NULL;
  char *token = NULL;
  bool searchOK = false;
  string::size_type FirstSharpPos = 0;
  ifstream in(file);
  if (!in.is_open())
  {
     //cout << file << " is not exist! " <<endl;
     //ERRORLOG(string(file) + "  is not exist!");
     return false;
  }
  while (!in.eof())    // reach the file end
  {
     in.getline(buf, 512);
     if ((FirstSharpPos = string(buf).find_first_of("#")) == string::npos) //there's none '#' in line
              ;
     else
        strcpy(buf, string(buf).erase(FirstSharpPos , 512).c_str());   // erase string from the first '#'
    
     if (strcmp(trim(trimbracket(trim(buf))).c_str(), section) == 0)  // find the dest section
     {
       //cout << "Find the section " << section << " correctlly" << endl;
       searchOK = true;
       first_ptr = NULL;
       break;
     }
  }
  if (!searchOK)          // not find the dest section
  {
     //cout << "Find the section " << section << " failed " <<endl; 
     //ERRORLOG(string(file) + "'s section " + section + " is not exist!");
     in.close();
     return false;
  }

  searchOK = false;       //start search the key
  while (!in.eof())    //
  {
     in.getline(buf, 512);
//#ifdef __DEBUG__
//     ILLog::Instance()->DebugLog(string("read line: ") + buf);
//#endif 
     if (strlen(trim(buf).c_str()) == 0)    // null line
         continue;
     if ( trim(buf).find("[")!=string::npos && trim(buf).find("]")!=string::npos )  // reach the next section
        break;
     if ((FirstSharpPos = string(buf).find_first_of("#")) == string::npos)  //there's none '#' in line
              ;
     else
        strcpy(buf, string(buf).erase(FirstSharpPos , 512).c_str());   // erase string from the first '#'
     token = strtok_r(buf, "=", &first_ptr);
     if (token == NULL)   // not correct line
        continue;
//#ifdef __DEBUG__
//     ILLog::Instance()->DebugLog(string("find key: Z!!") + token + "!!Z");
//#endif
     if (strcmp(trim(token).c_str(), key) == 0)      // find the correct key
     {
        //cout << "find the key " << key << " correctlly " <<endl; 
        //token = strtok_r(NULL, "=", &first_ptr);
        //if (token == NULL)               //key has no value
        //    break;
        if(strlen(first_ptr) > outbufLen) {
          //cout << "read content is too long for outbuf" << endl;
          in.close();
          return false;
        }
        memset(outbuf, '\0', outbufLen*sizeof(char));
        strcpy(outbuf, trim(first_ptr).c_str());
        searchOK = true;
     }
  }
  if (!searchOK)
  {
      //cout << "Find the key " << key << " failed " <<endl; 
      //ERRORLOG(string(file) + " section " + section + " doesn't have key " + key);
      in.close();
      return false;
  }
  in.close(); 
  return true;
}


CFlowBase::CFlowBase()
{

}

CFlowBase::~CFlowBase()
{
}

CDisposer::CDisposer()
{
    m_ConfigPath = CONFIGFILEPATH;
    m_nClientDelay = 0;//设置初始间断时间为0
    memset((void *)m_ClientArray, 0, MAX_CONNECTION * sizeof(CClient *));
}

CDisposer::~CDisposer()
{
    for (int i = 0; i < m_nClientSize; i++)
    {
        if (m_ClientArray[i])
        {
            delete m_ClientArray[i];
        }
    }
}

//从config.ini中读取全局参数
long CDisposer::ReadConfig(const char *strItem)
{
    char buf[256] = { '\0' };
    bool flag = true;
    long nRet =  0;
#ifndef DEBUGONWIN
    flag = ReadValueFromFile(m_ConfigPath.c_str(), "CONFIG", strItem, buf, 256);
    if(flag)
    {
        cout << strItem <<":" << buf << endl;
    }
    else
    {
        cerr << "Read Config.ini "<< strItem << " Fail" << endl;
        return 0;
    }
    nRet = atoi(buf);
#endif
    return nRet;
}

//读取Task项目的内容，需根据config项中的内容决定以下读取内容
bool CDisposer::ReadTaskInfo(CTask *task, int seq)
{
    char buf[256] = { '\0' };
    char strItem[128] = { '\0' };
    bool flag = true;
    cout << endl;
    sprintf(strItem,"TASK_%d",seq);
#ifndef DEBUGONWIN
    flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_UID].TName, buf, 256);
    task->SetUID(atoi(buf));
    cout << "UID:" << buf << endl;
    //读取游戏规则，内容规则和重复次数
    flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_RULES].TName, buf, 256);

    string rules(buf);
    cout << "Rules:" << buf << endl;
    flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_REPS].TName, buf, 256);
    task->SetRules(rules,atoi(buf));
    cout << "Reps:" << buf << endl;
    //读取游戏类型，并将游戏类型设置到任务中
    flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_GAMETYPE].TName, buf, 256);
    for (int i = 0; i < GAME_MAX_LENGTH; i++)
    {
        if (strcmp(buf, tGame[i].GName) == 0)
        {
            task->SetGameType(tGame[i].GType);
            break;
        }
    }
    cout << "Type:" << buf << endl;
    //读取游戏方法Method，并将其设置到任务中
    flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_METHOD].TName, buf, 256);
    for (int i = 0; i < METHODLENGTH; i++)
    {
        if (strcmp(buf, METHODARRAY[i].MName) == 0)
        {
            task->m_method = METHODARRAY[i].MethodId;
            break;
        }
    }
    cout << "Method:" << METHODARRAY[task->m_method].MethodId << endl;
    //读取URL
    flag = ReadValue(m_ConfigPath.c_str(), strItem, tableTask[TASK_URL].TName, buf, 256);
    task->SetURL(buf);
    cout << "URL:" << buf << endl;
    //读取wager issue
    //flag = ReadValueFromFile(m_ConfigPath.c_str(), strItem, tableTask[TASK_WI].TName, buf, 256);
    //if(flag)
    //{
    //    task->m_strWagerIssue = buf;
    //    cout << "WagerIssue:" << buf << endl;
    //}

#endif
    return flag;
}
void CDisposer::MainFlow()
{
    bool bflag = false;
    int clientSize = ReadConfig(tableConfig[CONFIG_CLIENT].itemName);
    int duration = ReadConfig(tableConfig[CONFIG_DURATION].itemName);
    int taskSize = ReadConfig(tableConfig[CONFIG_TASKSIZE].itemName);
    m_nClientSize = clientSize;
    CCrew crew;//create thread vector and task vector
    CTimer timer(&crew);//实例化时间控制类
    bflag = crew.CrewInit(clientSize, duration, m_nClientDelay);

    srandEx(time(NULL));//制作随机种子

    //创建客户和其线程，并将其加入到监控队列
    CClient *pClient = NULL;
    for (int i = 0; i < clientSize; i++)
    {
        pClient = new CClient(&crew);
        pClient->AttachConnect(NULL);//由模拟客户自动创建
        m_ClientArray[i] = pClient;
    }

    for (int i = 0; i < clientSize; i++)
    {
        bflag = (m_ClientArray[i])->CreateThread();
        if (true == bflag)
        {//将线程加入到监测池中，方便Mgr线程监控
            crew.AddThread(&((m_ClientArray[i])->m_handler));
        }
        else
        {
            return;
        }
    }

    timer.AttachTimer(duration);//创建timer线程，并使其等待start信号

    //读取任务信息，将任务加入到任务队列
    CTask *pTask = NULL;
    for (int i = 0; i < taskSize; i++)
    {
        pTask = new CTask();
        pTask->SetRandGameType();//在正式读取游戏类型钱，将游戏种类置为随机值
        ReadTaskInfo(pTask, i);
        crew.AddTask(pTask);
    }

    crew.JoinAllThread();//发送start信号，并等待线程退出
//    cout << "Time Elapse:" << timer.TimerElapse() << endl;
    timer.WaitForExit();
    crew.m_nTimeDuration = timer.TimerElapse() / 1000;
    cout << "Flow OutPut Statistic" << endl;
    OutPutStatics(crew);

    cout << "Flow End" << endl;
    //获取结果，分析数据
}

void CDisposer::PrintHelp()
{
    cout << program_name << " " << version_string << endl;
    cout << copyright << endl;
    cout << "Usage:";
    cout << "./MultiClients [-f File]" << endl;
    cout << "-f, --file=FILE           FILE,select a specific Confil FILE."  << endl;
}

void CDisposer::OutPutStatics(CCrew &crew)
{
    cout << endl;

    cout << "Total  Connection:" << crew.m_nTotalConnectionCount << endl;
    cout << "Total Transaction:" << crew.m_nTotalTransactionCount << endl;
    cout << "Total Time Elapse:" << crew.m_nTimeDuration << " ms" << endl;
    cout << "All Jobs Sum Time:" << crew.m_lTotalCaltime << " ms" << endl;
    cout << "Job Average Time :" << (float)crew.m_lTotalCaltime / crew.m_nTotalConnectionCount << " ms" << endl;
    cout << "Max Spend Time   :" << crew.m_nMaxClientTime << " ms  Connection:" << crew.m_nConCountWhenMaxTime << endl;
    cout << "Min Spend Time   :" << crew.m_nMinClientTime << " ms  Connection:" << crew.m_nConCountWhenMinTime << endl;
    float td = ((float)crew.m_nTimeDuration) / 1000;
    cout << "QueryTPS         :" << crew.m_nTotalConnectionCount / td << endl;
    cout << "Transaction TPS  :" << crew.m_nTotalTransactionCount / td << endl;
    for (int i = 0; i < ERRORCODE_LENGTH; i++)
    {
        cout << ERRORMSG[i].EName << ":" << crew.m_ResultCount[i] << endl;
    }
    cout << endl; //分界
    //Task Details
    for (int i = 0; i < crew.m_lTotalTaskSize; i++)
    {
        const CTask * task = crew.GetTaskByIndex(i);
        if(task == NULL)
            break;
        cout << "Task Index :" << i + 1 << " Name :" << tGame[task->GetGameType()].GName << endl;
        cout << "Total Connection :" << task->m_nTotalConnectionCount << endl;
        cout << "Total Transaction:" << task->m_nTotalTransactionCount << endl;
        //cout << "Total Time Elapse:" << task->m_nTimeDuration << endl;//单独task所消耗时间以后考虑
        cout << "Max Spend Time   :" << task->m_nMaxClientTime << " ms  Connection:" << task->m_nConCountWhenMaxTime << endl;
        cout << "Min Spend Time   :" << task->m_nMinClientTime << " ms  Connection:" << task->m_nConCountWhenMinTime << endl;
        for (int ij = 0; ij < ERRORCODE_LENGTH; ij++)
        {
            cout << ERRORMSG[ij].EName << ":" << task->m_ResultCount[ij] << endl;
        }
        cout << endl;
    }
}

bool CDisposer::ParseArgs(int argc, char *p_argv[])
{
    short count;
    const char *p_input;
    int container_set = 0;
    int err = 0;

    if (argc == 1)
        return true;
    // parse command line
    for (count = 1; count < argc; count++)
    {
        if (*(p_argv[count]) == '-')
        {
            p_input = p_argv[count] + 1;

            switch (*(p_input))
            {
            case 'h':
            case 'H':
                PrintHelp();
                return false;
            case 'd':
            case 'D':
                count++;
                m_nClientDelay = atoi(p_argv[count]);
                //strcpy(infilename, p_input + 1);
                break;
            case 'o':
            case 'O':
                //strcpy(, p_input + 1);
                break;

            case 'r':
            case 'R':
                break;

            case 't':
            case 'T':
                break;

            case 'f':
            case 'F':
                count++;
                m_ConfigPath = string(p_argv[count]);
                break;
            default:
                return false;
            }
        }
    }
    return true;
}