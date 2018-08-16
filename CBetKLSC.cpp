/**************************************************************
*Copyright(C),2016,CLO
*FileName:Game.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/20
*Description:游戏的具体实现在这个文件中实现
**************************************************************/
#include "libjson/libjson.h"
#include "ILTools.h"
#include "urlcode.h"
#include "Common.h"
#include "CBetKLSC.h"
#include<sys/time.h>
#include "clo-rand.h"
#define UIDBASE 10000000

//游戏停止投注时间，即为游戏算奖时间55S
#define BET_STOP_DURATION_TIME 55

bool CBetKLSC::isSemaInit = false;
pthread_mutex_t CBetKLSC::m_Trans_lock = PTHREAD_MUTEX_INITIALIZER;

//开乐彩游戏
void CBetKLSC::GameOperation(string strRules)
{
    //进行http访问
    string strResponse;
    m_bSuccess = false;
    static string strWagerIssue = "";
    static int curEndTime = 0;
    
    if(m_method == RANDOM)
    {//随机method
        m_method = METHOD(randEx() % METHODLENGTH);
    }
    if (m_method == QUERY)
    {
        strResponse = Request(m_strURL);
        m_nConnectionCnt++;
        if (strResponse == "")
        {
            m_error = CONNECT_ERROR;
            return;
        }
        m_error = SUCCESS;
        m_bSuccess = true;
        return;
    }
    //进行中奖查询
    if (m_method == PRIZE)
    {
        string tSN = "";
        strResponse = Request(m_strURL + "Method=TicketPrizeQuery&GameID=02&TicketSN=" + tSN + "&Version=1.0");
        m_nConnectionCnt++;
        if (strResponse == "")
        {
            m_error = CONNECT_ERROR;
            return;
        }
        m_error = SUCCESS;
        m_bSuccess = true;
        return;
    }
    //查询某期的中奖号码
    if (m_method == WINCODE)
    {
        string strWI = m_strWagerIssue;
        strResponse = Request(m_strURL + "Method=GetWinCode&GameID=02&WagerIssue=" + strWI + "&Version=1.0");
        m_nConnectionCnt++;
        if (strResponse == "")
        {
            m_error = CONNECT_ERROR;
            return;
        }
        m_error = SUCCESS;
        m_bSuccess = true;
        return;
    }
    //进行交易模拟
    int c = 0;
#ifndef DEBUGONWIN
    if ((c = pthread_mutex_lock(&(m_Trans_lock))) != 0)
    {
        //mutex lock
    }
#endif
    string strTime;
    getLocalTime(strTime, "ddhhmmss");
    int nTime = atoi(strTime.c_str());
    if(curEndTime < nTime + 5)
    {
        int nErrTime = 0;
        do
        {
            if(curEndTime != 0)
            {
                cerr << "Betting Stop Duration wait :" << BET_STOP_DURATION_TIME + 5 << " seconds" << endl;
                sleep(BET_STOP_DURATION_TIME + 5);//
            }
            strResponse = Request(m_strURL + "Method=GetGameParam&Version=1.0&GameID=01");
            nErrTime ++;
            m_nConnectionCnt ++;
            if (strResponse == "")
            {
                m_error = CONNECT_ERROR;
                break;
            }
            if (!libjson::is_valid(strResponse))//判断是否是有效的json格式
            {
                m_error = JSON_ERROR;
                break;
            }
            JSONNode resultNode = libjson::parse(strResponse);
            JSONNode::const_iterator iter = resultNode.find("ReturnCode");
            if (iter == resultNode.end())//返回值不包含ReturnCode
            {
                m_error = RETURN_CODE_ERROR;
                break;
            }

            if (iter->as_string() != "000000")   //返回值为0则获取参数成功，否则失败
            {
                cerr << "HLJN Return Code: " << iter->as_string() << endl;
                //如果错误次数太多，则退出
                if(nErrTime > 2)
                {
                    m_error = RETURN_CODE_ERROR;
                    break;
                }
                continue;
            }
            //返回正确错误码，开始解析投注期号
            if ((iter = resultNode.find("WagerIssue")) == resultNode.end())
            {
                m_error = GET_WAGER_ERROR;
                break;
            }
            else
            {
                strWagerIssue = iter->as_string();
            }
            //查找投注截止时间
            if ((iter = resultNode.find("FirstBetEndTime")) == resultNode.end())
            {
                m_error = GET_WAGER_ERROR;
                break;
            }
            else
            {
                char cArray[15] = {'\0'};
                sprintf(cArray,"%s",(iter->as_string()).c_str());
                curEndTime = atoi(cArray + 6);
                break;
            }
        }while(true);
    }

#ifndef DEBUGONWIN
    if ((c = pthread_mutex_unlock(&(m_Trans_lock))) != 0)
    {
        //mutex unlock
    }
    if(m_error != NOTHING)
    {
        return;
    }
#endif
    //有文件传入，每次自增1，自增过程在获取task时完成
    int nUID = m_nUID;//randEx() % UIDBASE;
    string strUID;
    INT_TO_CHAR(strUID, nUID);
    Transaction(strUID, strWagerIssue);
}

void CBetKLSC::Transaction(string lpszUID, string strWagerIssue)
{
    string strSerialID = "";
    string strResponse = "";
    //struct timeval tval;
    //gettimeofday(&tval, NULL);
    getLocalTime(strSerialID, "yyyyMMddhhmmss");
    string strThreadId;
    INT_TO_CHAR(strThreadId, m_nRandom);
    strSerialID += strThreadId;//添加线程ID唯一性
    string strIndex;
    INT_TO_CHAR(strIndex, m_nIndex);
    strSerialID += strIndex;//添加index自增唯一性
    int randNum = randEx() % 9000 + 1000;//tval.tv_usec % 8999 + 1000;
//    for(int i = 0 ; i < 10 ; i++) randNum += randEx() % 1000;
    string strTmp;
    INT_TO_CHAR(strTmp, randNum);
    strSerialID += strTmp;  //后四位加随机码
//    cerr << "SID: " << strSerialID << endl;
    int nCardIndex = 1;
    //四张卡号
    string strCardIndex;
    INT_TO_CHAR(strCardIndex, nCardIndex);

    //随机投1~50倍
    int nBetMulti = randEx() % 50 + 1;
    int nBetMoney = nBetMulti * 500;  //单位: 分
    string strBetMulti, strBetMoney;
    INT_TO_CHAR(strBetMulti, nBetMulti);
    INT_TO_CHAR(strBetMoney, nBetMoney);

    string strBetData = "";
    for (int i = 0; i < 1; i++) //只有1注
    {
        int nBetNumCount = 10;  //必须投10个

        int nTmpCount = nBetNumCount;
        for (int j = 0; j < 80; j++)  //从1到80随机选出nBetNumCount个不重复的数字
        {
            int nRandNum = randEx() % (80 - j);
            if (nRandNum <= nTmpCount)  //在选择范围内
            {
                //使用j做随机投注码
                char lpszTmp[3] = { '\0' };
                sprintf(lpszTmp, "%d", j + 1);
                if (strBetData.length() != 0)
                    strBetData += ",";
                strBetData += lpszTmp;

                nTmpCount--;
                if (nTmpCount <= 0)  //生成满足需求的个数了
                    break;
            }
        }
    }

    string strBetRequest = string("Method=Betting&Version=1.0&GameID=01&WagerIssue=") + strWagerIssue
        + string("&Amount=") + strBetMoney + string("&BetMulti=") + strBetMulti + string("&DrawWay=1&BetIndex=1&CardIndex=")
        + strCardIndex + string("&BetData=") + strBetData + string("&UID=") + lpszUID + string("&SerialID=") + strSerialID;

    strResponse = Request(m_strURL + strBetRequest);
    m_nConnectionCnt++;
    m_nTransactionCnt++;
    if (strResponse == "")
    {
        m_error = BET_ERROR;
        return;
    }
/*
    if ((randEx() % 10) < 5)  //一半的概率选择玩第三彩
    {
        m_error = SUCCESS;
        m_bSuccess = true;
        return;

    }
*/
    if (!libjson::is_valid(strResponse))  //如果不是json格式
    {
        m_error = JSON_ERROR;
        return;
    }
    JSONNode resultNode = libjson::parse(strResponse);
    JSONNode::const_iterator iter = resultNode.find("ReturnCode");
    if (iter == resultNode.end())
    {
        m_error = JSON_ERROR;
        return;
    }
    
    if (iter->as_string() != "000000")
    {
        cerr << "KLSC UID:" << lpszUID <<" SerialID:" << strSerialID <<" Return Code: " << iter->as_string() << endl;
        m_error = RETURN_CODE_ERROR;
        return;
    }
    m_error = SUCCESS;
    m_bSuccess = true;
/*
    string strTicketSN;
    //返回正确错误码，开始解析投注期号
    if ((iter = resultNode.find("TicketSN")) == resultNode.end())
    {
        m_error = JSON_ERROR;
        return;
    }
    else
    {
        strTicketSN = iter->as_string();
    }

    string strThirdBetRequest = string("Method=Betting&Version=1.0&GameID=01&TicketSN=")
        + strTicketSN + string("&ThirdBetFlag=Y&WagerIssue=") + strWagerIssue + string("&BetIndex=3");

    strResponse = Request(m_strURL + strThirdBetRequest);
    m_nConnectionCount++;
    m_nTransactionCount++;
    if (strResponse == "")
    {
        m_error = BET_ERROR;
    }
    else {
        m_error = SUCCESS;
        m_bSuccess = true;
    }
*/
}