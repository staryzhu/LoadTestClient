/**************************************************************
*Copyright(C),2016,CLO
*FileName:Game.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/4/30
*Description:欢乐基诺游戏实现
**************************************************************/
#include "libjson/libjson.h"
#include "ILTools.h"
#include "urlcode.h"
#include "Common.h"
#include "CBetHLJN.h"
#include<sys/time.h>
#include "clo-rand.h"

#define UIDBASE 10000000

//游戏停止投注时间，即为游戏算奖时间25S
#define BET_STOP_DURATION_TIME 25


bool CBetHLJN::isSemaInit = false;
pthread_mutex_t CBetHLJN::m_Trans_lock = PTHREAD_MUTEX_INITIALIZER;

//欢乐基诺
void CBetHLJN::GameOperation(string strRules)
{
    //进行http访问
    string strResponse;
    m_bSuccess = false;
    m_nTransactionCnt = 0;
    m_nConnectionCnt = 0;
    static string strWagerIssue = "";
    static int curEndTime = 0;
    
    if(m_method == RANDOM)
    {//随机method
        m_method = METHOD(randEx() % METHODLENGTH);
    }
    //进行查询
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
            strResponse = Request(m_strURL + "Method=GetGameParam&Version=1.0&GameID=02");
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
            if ((iter = resultNode.find("DrawEndTime")) == resultNode.end())
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
//    cout << "HLJN" << endl;
}

void CBetHLJN::Transaction(string lpszUID, string strWagerIssue)
{
    //模拟一个人一次投随机次(小于40)盘
    string strTicketBetData;
    string strResponse = "";
    int nTicketBetMoney = 0;
    const int g_MoneyCanBet[6] = { 10, 50, 100, 200, 500, 1000 };
    const int g_MaxBetMulti = 99;
    int nBetPlateCount = randEx() % 40 + 1;
    for (int i = 0; i < nBetPlateCount; i++)
    {
        //单盘的选择单注投注额
        int nSingleBetMoney = g_MoneyCanBet[randEx() % 6];
        //单盘的投注倍数
        int nSingleBetMulti = randEx() % 99 + 1;
        //单盘的投注额
        int nPlateBetMoney = nSingleBetMoney * nSingleBetMulti;
        //累加票的投注额
        nTicketBetMoney += nPlateBetMoney;
        //选择单盘的投注号码
        string strPlateBetData = "";

        int nBetNumCount = 5;  //投5个号码

        int nTmpCount = nBetNumCount;
        for (int j = 0; j < 80; j++)  //从1到80随机选出nBetNumCount个不重复的数字
        {
            int nRandNum = randEx() % (80 - j);
            if (nRandNum <= nTmpCount)  //在选择范围内
            {
                //使用j做随机投注码
                char lpszTmp[3] = { '\0' };
                sprintf(lpszTmp, "%d", j + 1);
                if (strPlateBetData.length() != 0)
                    strPlateBetData += ",";
                strPlateBetData += lpszTmp;

                nTmpCount--;
                if (nTmpCount <= 0)  //生成满足需求的个数了
                    break;
            }
        }
        string strSingleBetMoney, strSingleBetMulti, strPlateBetMoney, strPlateIndex;
        INT_TO_CHAR(strSingleBetMoney, nSingleBetMoney);
        INT_TO_CHAR(strSingleBetMulti, nSingleBetMulti);
        INT_TO_CHAR(strPlateBetMoney, nPlateBetMoney);
        INT_TO_CHAR(strPlateIndex, i + 1);

        if (strTicketBetData.length() > 0)
            strTicketBetData += "#";
        strTicketBetData += strPlateIndex + "|" + strPlateBetMoney + "|" + strSingleBetMoney + "|" +
            strSingleBetMulti + "|" + strPlateBetData;
    }

    string strTicketBetMoney, strBetPlateCount;
    INT_TO_CHAR(strTicketBetMoney, nTicketBetMoney);
    INT_TO_CHAR(strBetPlateCount, nBetPlateCount);

    string strSerialID = "";
    getLocalTime(strSerialID, "yyyyMMddhhmmss");
    //int randNum = randEx() % 8999 + 1000;
    //string strTmp;
    //INT_TO_CHAR(strTmp, randNum);
    strSerialID += lpszUID;  //后四位加随机码,修改为加入UID

    string strBetRequest = string("Method=Betting&Version=1.0&GameID=02&WagerIssue=") + strWagerIssue
        + string("&Amount=") + strTicketBetMoney + string("&DrawWay=1&BetPlateCount=") + strBetPlateCount
        + string("&BetData=") + URLEncode(strTicketBetData) + string("&UID=") + lpszUID + string("&SerialID=") + strSerialID;
    strResponse = Request(m_strURL + strBetRequest);
    m_nConnectionCnt++;
    m_nTransactionCnt++;
    if (strResponse == "")
    {
        m_error = BET_ERROR;
        return;
    }
    else 
    {
        //m_error = SUCCESS;
        //m_bSuccess = true;
    }
    //此处判定结果是否正确
    if (!libjson::is_valid(strResponse))//判断是否是有效的json格式
    {
        m_error = JSON_ERROR;
        return;
    }
    JSONNode resultNode = libjson::parse(strResponse);
    JSONNode::const_iterator iter = resultNode.find("ReturnCode");
    if (iter == resultNode.end())//返回值不包含ReturnCode
    {
        m_error = JSON_ERROR;
        return;
    }

    if (iter->as_string() != "000000")   //返回值为0则获取参数成功，否则失败
    {
        //cerr << "HLJN Return Code: " << iter->as_string() << endl;
        cerr << "HLJN UID:" << lpszUID <<" Return Code: " << iter->as_string() << endl;
        m_error = RETURN_CODE_ERROR;
        return;
    }
    m_error = SUCCESS;
    m_bSuccess = true;
}
