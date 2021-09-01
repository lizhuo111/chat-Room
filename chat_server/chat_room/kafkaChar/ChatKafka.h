#pragma once
#include "kdvtype.h"
#include "mqinterface.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <time.h>
#include <sys/timeb.h>
#ifdef _MSC_VER
#include <windows.h> 
#include "wingetopt.h"
#include <process.h>
#include "pthread_w32.h"
#else
#include <unistd.h>
#include <pthread.h>
#include <getopt.h>
#endif
#ifdef _MSC_VER
#define sleep(nseconds) Sleep(nseconds * 1000);
#endif
#ifdef TOOL_
#define TOOL_
#endif // 
static int g_nMsgCount = 0;
static long g_lLastOffset = 0;

static void* ReConnectThread(void* args)
{
    MQHANDLE hServer = (MQHANDLE)(long)args;
    if (0 == hServer)
    {
        return NULL;
    }
    sleep(1);
    while (FALSE == MQReConnectServer(hServer, 1000))
    {
        sleep(10);
    }
    printf("reconnect server success\n");
    return 0;
}

static void eventcb(MQTOPICHANDLE handle, s32 nErrno, s8* pszErr, s32 nErrLen, void* pUser)
{
    printf("\tEVENT:handle:%X err:%d errstr:%s\n", handle, nErrno, pszErr);
    if (nErrno == MQ_ERR_SERVER_DOWN)
    {
        printf("eventcb server down ,will reconnect after 1s\n");
    }

}

static u32 precmsgcb(MQTOPICHANDLE handle, PTMQMessage pMessage, int partition, long offset, void* pUser)
{
    ++g_nMsgCount;
    printf("\tmessage:Handle=0x%X topic:%s time:%d:%d pattition:%d offset:%d ",
        handle, pMessage->szTopic, pMessage->timestamp.time, pMessage->timestamp.millitm, partition, offset);
    std::string strKey((const char*)pMessage->pszKey, pMessage->nKeyLen);
    printf("key:len=%d,val=%s, value:len=%d,val=%s\n",
        pMessage->nKeyLen, 0 != pMessage->nKeyLen ? (const u8*)strKey.c_str() : (const u8*)"", pMessage->nValueLen, pMessage->pszValue);
    //printf("recv total msg:%d\n", g_nMsgCount);
    long g_lLastOffset = 0;
    if (g_nMsgCount % 3 == 1)
    {
        g_lLastOffset = offset;
    }
    if (g_nMsgCount % 3 == 2)
    {
        //         MQReceiverSeek(handle, pMessage->szTopic, partition, g_lLastOffset, 1);
        //     }
        //     else
        //     {
        MQReceiverCommit(handle, pMessage->szTopic, partition, offset, true);
    }
    return 0;
}

static void commitcb(MQTOPICHANDLE handle, const char* topic, int partition, long offset, int errCode, void* pUser)
{
    printf("\tcommitcb:Handle=0x%X pattition:%d offset:%d errcode:%d\n",
        handle, partition, offset, errCode);
}
static void ReConnAfter1s(MQHANDLE hServer)
{
    //TODO check mqkafkaserver has been down or not
    //创建线程用来进行延迟处理，防止主线程阻塞导致后续行为出错
    pthread_t tThread;
    // 创建detached线程 
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&tThread, &attr, ReConnectThread, (void*)hServer);
    if (ret != 0)
    {
        printf("create ReConnectThread failed: error_code=%d\n", ret);
    }
    pthread_attr_destroy(&attr);
}


static void msgcb(MQTOPICHANDLE handle, PTMQMessage pMessage, void* pUser)
{
    ++g_nMsgCount;
    printf("\tmessage:Handle=0x%X topic:%s time:%d:%d",
        handle, pMessage->szTopic, pMessage->timestamp.time, pMessage->timestamp.millitm);
    std::string strKey((const char*)pMessage->pszKey, pMessage->nKeyLen);
    printf("key:len=%d,val=%s, value:len=%d,val=%s\n",
        pMessage->nKeyLen, 0 != pMessage->nKeyLen ? (const u8*)strKey.c_str() : (const u8*)"", pMessage->nValueLen, pMessage->pszValue);
    //printf("recv total msg:%d\n", g_nMsgCount);
}

static void downcb(MQHANDLE handle, void* pUser)
{
    printf("downcb server down ,will reconnect after 1s\n");
    /*ReConnAfter1s(MQGetServerHandle(handle));*/
    ReConnectThread((void*)handle);
}
static void logcb(eMQLoglev eLevel, const s8* pszLog, u32 nLogLen, void* pUser)
{
    /*if(MQ_LOG_NORMAL_LEVEL < eLevel)*/
    printf("LOG:lev=%d %s", eLevel, pszLog);
}
static bool run = true;
class ChatKafka
{public:
    ChatKafka();
 






    void CheckKey(bool bIsKey)
    {
        if (bIsKey) { printf("enter message key:\n"); }
        else { printf("enter message value:\n"); }
    }
    void SendMsg(std::string& key,std::string& value);
    std::string* GetMsg();
private:
    PTMQServerInfo ptServerInfo;
    PTMQTopicInfo ptTopicInfo;
    MQHANDLE hServer;
    std::string top;
};

