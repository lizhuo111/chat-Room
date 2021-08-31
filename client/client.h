#pragma once
#include<sys/fcntl.h>
#include <dirent.h>
#include<iostream>
#include<string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include"hiredis/hiredis.h"
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<vector>

// server port
#define SERVER_PORT 8888

//epoll 支持的最大并发量
#define EPOLL_SIZE 5000

//message buffer size
#define BUF_SIZE 0xFFFF

// exit
#define EXIT "EXIT"


using namespace std;
class client
{
public:
	//设置sockfd，pipefd非阻塞
	int setnonblocking(int sockfd) {
		fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
		return 0;
	}

	int addfd(int epollfd, int fd, bool enable_et) {
		struct epoll_event ev;
		ev.data.fd = fd;
		ev.events = EPOLLIN; //输入出发epoll-event
		if (enable_et) {
			ev.events = EPOLLIN | EPOLLET;
		}
		epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
		setnonblocking(fd);
	}
	client();
	
	void downloadFile();

	~client();

	bool connRedis();
	/*void exitRedis();
	
	/*bool DownloadFile(int fileId);
	void chatPub();
	void chatPubExit();
	void chatPrivate(string username);
	void chatPriExit();*/
	void Run();
	void logIn();
	void wellcome();

	void chatToSomeone();
	int function1();

	string sendmsg();
	void chatToSomeoneTool(string &username);
	void getinfor();
	void chat();
	void do_work();
	bool isCON;
	void sendFile();
	void getFileExists(vector<string>& filenames);
private:
	std::string username;
	bool isConnect;
	redisContext* pRedisContext;
	struct stat buf;
	int sock;
	std::string addr;
	struct sockaddr_in seraddr;
	int port;
	std::string msg;
	std::string msgrev;
	int func;
	string myUserName;
	bool login;
	vector<string> filenames;
	
};

