
#include"../epoller/epoller.h"
#include"../pool/threadpool.h"	
#include"../pool/pool.h"
#include<string>
#include<string.h>
#include"hiredis/hiredis.h"
#include<iostream>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"../pool/sqlconnRALL.h"
#include<unordered_map>
#pragma once

class server
{	
public:
	server(int port,  int connPoolNum, int threadNum):threadpool_(new threadpool(threadNum)), epoller_(new Epoller()) {
		this->port = port;
	
		pool::Instance()->Init(LOCAL_HOST, 3306, "root", "123456", "chatroom", 10);
	}
	~server();
	//void sendFile(string filename);
	void start();

	bool redisCon();
	string fileExits(string fileName);
	/*bool recFile();*/
	void sqlConn();
	bool initServer();
	bool userVerify(string& user);
	int JudgeCmd(string& Data);
	void DealListen_();
	void ReadFd(int fd);
	void writeFd(int fd);
	int SetFdNonblock(int fd);

private:
	int port; int connPoolNum; int threadNum;
	std::unique_ptr<threadpool> threadpool_;
	std::unique_ptr<Epoller> epoller_;
	redisContext* pRedisContext;
	int listenfd;
	uint32_t listenEvent_;
	uint32_t connEvent_;
	string msg;
	std::string response;
};

