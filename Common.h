/**************************************************************
*Copyright(C),2016,CLO
*FileName:Common.h
*Author:Stary(ZhiGuang) Zhu
*Version:1.0
*Date:2016/3/28
*Description:存放自定义类型
**************************************************************/
#ifndef COMMON_H
#define COMMON_H 
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
//总体配置项目的信息条目
typedef enum
{
    CONFIG_CLIENT = 0x0,
    CONFIG_DURATION,
    CONFIG_TASKSIZE,
    CONFIG_LENGTH
}CONFIGINFO;

typedef struct
{
    CONFIGINFO configInfo;
    char itemName[256];
}CONFIGNAME;

const CONFIGNAME tableConfig[CONFIG_LENGTH] =
{
    { CONFIG_CLIENT, "client" },
    { CONFIG_DURATION, "duration" },
    { CONFIG_TASKSIZE, "tasksize" },
};

//任务task的信息条目
typedef enum
{
    TASK_UID = 0x0,
    TASK_GAMETYPE,
    TASK_RULES,
    TASK_REPS,
    TASK_URL,
    TASK_METHOD,
    TASK_WI,
    TASK_LENGTH
}TASKINFO;

typedef struct
{
    TASKINFO taskInfo;
    char TName[256];
}TASKNAME;

//此处字符串与配置文件config.ini中的TASK信息项对应
const TASKNAME tableTask[TASK_LENGTH] = 
{
    { TASK_UID, "uid" },
    { TASK_GAMETYPE, "gametype" },
    { TASK_RULES, "rules" },
    { TASK_REPS, "rep" },
    { TASK_URL, "url" },
    { TASK_METHOD, "method"},
    { TASK_WI, "wagerissue"},
};

//Define for Game Type在此处添加游戏种类
typedef enum
{
    GAME_TRANSACTION = 0x00,
    GAME_HLJN        = 0x01,
    GAME_KLSC        = 0x02,
    GAME_MAX_LENGTH
}GAMETYPE;

typedef struct
{
    GAMETYPE GType;
    char GName[256];
}GAMENAME;

//游戏的名字，在config.ini中可以读取，并匹配
const GAMENAME tGame[GAME_MAX_LENGTH] =
{
    { GAME_TRANSACTION, "TRANSACTION" },
    { GAME_HLJN, "HLJN" },
    { GAME_KLSC, "KLSC" },
};

//模拟交易时，出现的错误代码
typedef enum
{
    NOTHING = 0x00,
    SUCCESS = 0x01,
    CONNECT_ERROR = 0x02,
    JSON_ERROR,
    RETURN_CODE_ERROR,
    GET_WAGER_ERROR,
    TRANSACTION_ERROR,
    BET_ERROR,
    ERRORCODE_LENGTH
}ERRORCODE;

typedef struct
{
    ERRORCODE ECode;
    char EName[128];
}RESULTNAME;

typedef struct
{
    ERRORCODE ECode;
    int count;
}RESULTCOUNT;

//游戏的名字，在config.ini中可以读取，并匹配
const RESULTNAME ERRORMSG[ERRORCODE_LENGTH] =
{
    { NOTHING, "OP Nothing"},
    { SUCCESS, "OP Success" },
    { CONNECT_ERROR, "Conection ERROR" },
    { JSON_ERROR, "Json ERROR" },
    { RETURN_CODE_ERROR, "ReturnCode ERROR" },
    { GET_WAGER_ERROR, "Get Wager ERROR" },
    { TRANSACTION_ERROR, "Transaction ERROR" },
    { BET_ERROR, "Bet ERROR" },
};

typedef struct
{
    int nTransactionCount;
    int nConnectionCount;
    bool bIsSuccess;//
    ERRORCODE errorInfo;
}RESULTINFO;

typedef enum
{
    RANDOM = 0x00,
    QUERY = 0x01,
    BETTING,
    WINCODE,
    PRIZE,
    OTHER,
    METHODLENGTH
}METHOD;

typedef struct
{
    METHOD MethodId;
    char MName[128];
}METHODNAME;

const METHODNAME METHODARRAY[METHODLENGTH] =
{
    { RANDOM, "Random" },
    { QUERY, "Query" },
    { BETTING, "Betting" },
    { WINCODE, "WinCode" },
    { PRIZE, "QueryPrize" },
    { OTHER, "Other" },
};

//配置文件和内容设置文件
#define CONFIGFILEPATH "./config.ini"

//输出文件位置
#define RESULTFILEPATH "./result.csv"

//Coding on windows Platform在Windows下进行流程调试
//#define DEBUGONWIN 

#ifdef DEBUGONWIN 
#define pthread_t int
#endif

#define ERROR stderr

//定义允许的最大连接数量，如果超过则选默认值
#define MAX_CONNECTION 1024

//全局设置，控制整体流程
extern int global_shutdown;

#endif/*COMMON_H*/
