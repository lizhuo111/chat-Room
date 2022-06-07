#include "server.h"

bool server::redisCon()
{
    this->pRedisContext = (redisContext*)redisConnect("127.0.0.1", 6379);
    
    if (pRedisContext->err != 0)  
        
    {cout << "redis err";

     return false;
    }
    else { std::cout << "success"; return true; } 
	
}
string server::fileExits(string fileName)
{
    redisReply* reply;
    string filePath;
    reply = (redisReply*)redisCommand(pRedisContext, "hget fileHash %s", fileName);
    filePath = reply->str;
    if (!filePath.empty())   cout<<"No file";
    return filePath;
}
/*数据库连接池初始化*/
void server::sqlConn()
{   
    pool::Instance()->Init(LOCAL_HOST, 3306, "root", "123456", "chatroom",10);
    return;
}

/*初始化工作  把端口绑定了 */
bool server::initServer()
{
    int ret;
    struct sockaddr_in addr;
    if (port > 65535 || port < 1024) {
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    /* 端口复用
    /* 只有最后一个套接字会正常接收数据。 */
    ret = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if (ret < 0)   std::cout << "err set refused"<< strerror(errno);
    ret = bind(listenfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        cout << "bind err";
        close(listenfd);
        return false;
    }

    ret = listen(listenfd, 6);
    if (ret < 0) {
        cout << "listen err"<< strerror(errno);
        close(listenfd);
        return false;
    }
    ret = epoller_->AddFd(listenfd, EPOLLIN);
    if (ret == 0) {
        strerror(errno);
        close(listenfd);
        return false;
    }
    
    //非阻塞
    if (SetFdNonblock(listenfd) < 0) {
        int flag = 0; return false;
    }

    return true;
}

/*登录验证 密码错误或者没有用户就返回f*/
bool server::userVerify(string& data)
{
    string userName;
    string pwd;
    char* ptr = (char*)data.c_str();
    char* cmd = strtok(ptr, ",");
    std::vector<string>UserInfo;
    //cout<<cmd<<endl;
    //sql.push_back(cmd);
    while (ptr = strtok(NULL, ","))
    {
        //cout<<ptr<<endl;
        UserInfo.push_back(ptr);
    }
    MYSQL* sql;
    SqlConnRAII(&sql, pool::Instance());
    sql = mysql_init(sql);
    if (!sql) { std::cout << "err"; return false; }
    sql = mysql_real_connect(sql, LOCAL_HOST, "root", "123456", "chatroom", 3306, nullptr, 0);
    if (!sql) { std::cout << "err"; return false; }
    bool flag = false;
    unsigned int j = 0;
    char order[256] = { 0 };
    MYSQL_FIELD* fields = nullptr;
    MYSQL_RES* res = nullptr;

    /* 查询用户及密码 */
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1",UserInfo[0].c_str());  //将查询语句写入order
   
    if (mysql_query(sql, order)) {      //查询操作  查询成功返回0
        mysql_free_result(res);        //释放res  res为查询结果集分配的内存
        return false;
    }
    res = mysql_store_result(sql);       //对成功检索数据的保存
    if (MYSQL_ROW row = mysql_fetch_row(res)) {
        string password(row[1]);
        if (UserInfo[1] == password) { flag = true; }
        else {
            flag = false;
            std::cout << "pwd false";
        }
    }
    else{
        flag = false;
    }
    mysql_free_result(res);
    pool::Instance()->FreeConn(sql);
    
    return flag;
}

int server::JudgeCmd(string& Data)
{
    switch (Data[0])
    {
    case '1':
        return 1;
        break;
    case '2':
        return 2;
        break;
    case '3':
        return 3;
        break;
    }
    return 0;
}

void server::DealListen_() {              //处理监听  这个处理监听再看addclinet操作
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
                                      //持续处理监听
        int fd = accept(listenfd, (struct sockaddr*)&addr, &len);
        std::cout << fd;
        if (fd <= 0) { return; }          
        epoller_->AddFd(fd, EPOLLIN);
    
}


void server::ReadFd(int fd)
{
    string ReData;
    
    int readerro;
    int res = read(fd, (void*)ReData.c_str(), 1024);
    if (res < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            cout << "err read";
            epoller_->DelFd(fd);
            return;
        }
    }
    epoller_->AddFd(fd, EPOLLIN);
    int cmd = JudgeCmd(ReData);
    switch (cmd)
    {

    case 1: { //log in
        if (userVerify(ReData)) {
            std::string a = "1";
            int erro = send(fd, (void*)a.c_str(), a.size(), 0);
            if (erro < 0)  std::cout << "err";
        }
        else {
            std::string a = "2";
            int erro = send(fd, (void*)a.c_str(), a.size(), 0);
            if (erro < 0)  std::cout << "err";
        }}

    case 3: {


    }

    }
    return;
}

void server::writeFd(int fd) {
    std::string response;
    int res = send(fd, (void*)response.c_str(), response.size()+1,0);
    if (res == 0)  epoller_->ModFd(fd, connEvent_|EPOLLIN);

}
int server::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}





server::~server()
{
    close(listenfd);
}

void server::start() {
    int timeMS = -1;  /* epoll wait timeout == -1 无事件将阻塞 */
    initServer();
    while (1) {
        int eventCnt = epoller_->Wait(timeMS);
        std::cout << "evNum" << eventCnt;
        for (int i = 0; i < eventCnt; i++) {
            /* 处理事件 */
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);
            if (fd == listenfd) {
                DealListen_();
            }
            else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
                assert(fd > 0);
                epoller_->DelFd(fd);
            }
            else{
                //threadpool_->AddTask(std::bind(&server::ReadFd, this, fd));
                ReadFd(fd);
              
            }
            //else if (events & EPOLLOUT) {
            //    //threadpool_->AddTask(std::bind(&server::writeFd,this,fd));
            //    writeFd(fd);
            //
            //    
            //}
        }
    }
   
}






























/*void conSer::epoll_work()
    {
        struct epoll_event ev,events[EPOLL_SIZE];
        int epfd = epoll_create(EPOLL_SIZE);
        ev.events = EPOLLIN;
        ev.data.fd = listenfd;
        epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev);

        while(1)
        {
            int events_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
            int i=0;

            for(;i<events_count;++i)
            {
                if(events[i].data.fd == listenfd)
                {
                    int conn;
	                socklen_t peerlen = sizeof(peeraddr);
                    pid_t pid;
                    
                    system("clear");
                    while((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen))>0)
                    {
                        cout<<endl<<"=============== Service ================"<<endl;
                        cout<<" EPOLL:Received New Connection Request "<<endl;
                        cout<<"  confd="<<conn;
                        cout<<"  ip="<<inet_ntoa(peeraddr.sin_addr);
                        cout<<"  port="<<ntohs(peeraddr.sin_port)<<endl;
                        cout<<"========================================"<<endl;
                        
                        pid = fork();
                        if(pid == -1)
                            ERR_EXIT("fork");
           
                        if(pid == 0)
                        {
                            close(listenfd);
                            cout<<"Start do_service"<<endl;
                
                            do_service(conn);

                            cout<<"End of do_service"<<endl;
                            cout<<"========================================"<<endl;
                            exit(EXIT_SUCCESS);
                        }else
                            close(conn);    
                    }
                }
            }
        }
    }*/