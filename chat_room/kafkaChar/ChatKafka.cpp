#include "ChatKafka.h"

ChatKafka::ChatKafka()
{
    if (!MQInit(logcb, downcb, NULL))
    {
        printf("mqinit failed!\n");
    }
    else {
        printf("mqinit suc");
    }
    ptServerInfo = new TMQServerInfo[1];
    std::string addr = "127.0.0.1";
    int port = 9092;
    strcpy(ptServerInfo[0].szServerHost,addr.c_str());
    ptServerInfo[0].nServerPort = port;
    printf("login to server\n");
    u32 topicCount = 0;
    hServer = MQLoginServer(ptServerInfo, 1, NULL, NULL, 1000);
    if (0 == hServer)
    {
        printf("error:login to server failed!\n");
    }
    printf("login server success,handle=%d\n", hServer);
    ptTopicInfo = new TMQTopicInfo[1];
    top="chatroom";
    strcpy(ptTopicInfo[0].szTopicName,top.c_str());
}
//发信息
void ChatKafka::SendMsg(std::string& key,std::string& value)
{
    std::cout << "send start"<<std::endl;
    TMQSenderParam tSenderParam;
    tSenderParam.pEventCb = eventcb;
    //strcpy(tSenderParam.szName, clientname.c_str());
    printf("topic is %s", ptTopicInfo[0].szTopicName);
    MQTOPICHANDLE hSender = MQCreateSender(hServer, top.c_str(), tSenderParam, 5000);
    if (0 == hSender)
    {
        printf("create sender for topic:%s failed!\n", top.c_str());
    }
    //控制台获取要发送的数据
    //Read messages from stdin and produce to broker.
    bool bIsKey = true;
    const u8* pszkey = NULL;
    const u8* pszval = NULL;
    s32 nKeyLen = 0;
    s32 nValLen = 0;
    if (!key.empty())
    {
        pszkey = (const u8*)key.c_str();
        nKeyLen = key.length() + 1;
    }
    pszval = (const u8*)value.c_str();
    nValLen = value.length() + 1;
    //send message
    bool bsync = false;
    int nRet = MQSendMessage(hSender, pszval, nValLen, pszkey, nKeyLen, bsync, 0, 1000);
    std::cout << "blabla" << std::endl;
    if (0 != nRet)
    {
        printf("send message failed!err:%d,errstr:%s, key:%s, val:%s\n", nRet, MQGetErrDesc(nRet), pszkey, pszval);
    }
    else { printf("send message suc!:%d,str:%s, key:%s, val:%s\n", nRet, MQGetErrDesc(nRet), pszkey, pszval); }

    std::cout << nRet << std::endl;
}

std::string* ChatKafka::GetMsg()
{
    TMQPreciseReceiverParam tReceiverParam;
    tReceiverParam.nMsgTimeout = 1000;
    tReceiverParam.pEventCb = eventcb;
    tReceiverParam.pMessageCb = precmsgcb;
    tReceiverParam.bAutoCommit = false;
    tReceiverParam.pCommitCb = commitcb;
    MQTOPICHANDLE hReceiver = MQCreatePreciseReceiver(hServer, ptTopicInfo, 1, tReceiverParam, 5000);
    if (0 == hReceiver)
    {
        printf("ERROR:create receiver failed!\n");
    }
    MQStartReveive(hReceiver);
    //开始接收数据
    //获取并打印receiver信息
    TMQReceiverInfo tInfo;
    MQGetReceiverInfo(hReceiver, tInfo);
    printf("receiver1 clientname:%s,group:%s, serverhandle:%d, topicnum:%d :\n",
        tInfo.szName, tInfo.szGroup, tInfo.hServer, tInfo.dwTopicInfoArrNum);
    for (int j = 0; j < tInfo.dwTopicInfoArrNum; ++j)
    {
        printf("\ttopic %d: %s\n", j, tInfo.tTopicInfoArr[j].szTopicName);
    }
    printf("recv started!\n");
    //         //停止接收数据
    //         MQStopReceive(hReceiver);
    //         MQDeleteReceiver(hReceiver);
    //         MQLogoutServer(hServer);
    while (run)
    {
        sleep(1);
    }
    return nullptr;
}
