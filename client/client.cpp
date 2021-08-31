#include "client.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include"stdio.h"
#include"sys/stat.h"
#include<sstream>
#include<fstream>
#include <string.h>
#include<condition_variable>
#include<mutex>
#include<queue>
#include<thread>

using namespace std;

client::client()
{
    using namespace std;
    port = 6666;
    addr = "127.0.0.1";
    const char* a = addr.c_str();
    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "err " << endl;
        
    }
    memset(&seraddr, 0, sizeof(seraddr));
    seraddr.sin_family = AF_INET;
    seraddr.sin_port = htons(port);
    seraddr.sin_addr.s_addr = inet_addr(a);//htonl(INADDR_ANY);
    //cout<<"client successful!"<<endl;
}
//�����ļ�
void client::downloadFile()
{
   
    char rec[120];
    int ret=send(sock, (msg.c_str()), msg.size() + 1, 0);
    cout << "�������ļ�Ϊ" << endl;
    ret=recv(sock, rec, 100, 0);
    string s(&rec[0], &rec[strlen(rec)]);
  
    
    
    if(s.size() ==0) {
        cout << "�޿����ļ�" << endl;
        return;
    }
    else {
        cout << s;
        cout << "������Ҫ���ص��ļ�" << endl;
        //����
        string df;
        cin >> df;
        string q;
        q = "5," + df;
        int res = send(sock, (q.c_str()), q.size(), 0);
        FILE* fp;
        fp = fopen(df.c_str(), "ab");
        char  buff[4096];

        int n = recv(sock, buff, 4096, 0);
        cout << buff;
        fwrite(buff, 1, n, fp);

        fclose(fp);
        cout << "�������" << endl;
        return;
    }
    

}

client::~client() {}

bool client::connRedis()
{
    this->pRedisContext = (redisContext*)redisConnect("127.0.0.1", 6379);

    if (pRedisContext->err != 0)

    {
        cout << "redis err";

        return false;
    }
    else { return true; }
}

void client::Run()
{   
    if (connect(sock, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0)
    {
        std::cout << "���Ӵ���"<<endl;
        return;
    }
    
       
     do_work();
    
}
//zhuce
void client::logIn()
{
    std::string user, passwd;

    std::cout << "ע���û�����";
    std::cin >> user;
    msg += user;
    std::cout << "��  �룺";
    std::cin >> passwd;
    //msg = msg + " "+passwd;
    msg = msg + "," + passwd;
    //cout<<"send msg = "<<msg<<endl;
    sendmsg();
    if (strncmp("1", msgrev.c_str(), 1) == 0)
    {
        function1();
        do_work();
        func = 0;
    }
    else if (strncmp("2", msgrev.c_str(), 1) == 0)
    {
        cout << "�û�����������";
        wellcome();
        do_work();
    }
    else
    {
        system("clear");
        std::cout << "δ֪����" << std::endl;
        //        //cout<<msg.c_str()<<endl;
        wellcome();
        do_work();
    }
}

void client::wellcome()
{
    using namespace std;
    //system("clear");
    cout << "====================================" << endl;
    cout << "=   Welcome !    =" << endl;
    cout << "====================================" << endl;
    cout << "=                                  =" << endl;
    cout << "=       1.��¼       2.ע��       =" << endl;
    cout << "=                                  =" << endl;
    cout << "====================================" << endl;
    cout << "Your Choice >: ";
    func = 0;
}
//˽��
void client::chatToSomeone()
{
    msg += "1,";
    msg += myUserName;
    send(sock, msg.c_str(), msg.size() + 1,0);
    char  buff[4096];
    cout << "������ȡ������Ϣ"<<endl;
    int res = 0;
   /* res = recv(sock, buff, 200, 0);
    int count = buff[0]-'0';
    int i = 0;
    while (i<count) {
       res = recv(sock, buff, 200, 0);
       string userPullMsg(&buff[0], &buff[strlen(buff)]);
       cout << userPullMsg << endl;
    }*/

    res = recv(sock, buff, 2000, 0);
    string userPullMsg(&buff[0], &buff[strlen(buff)]);
    cout << userPullMsg<<endl;
    cout << "������ȡ�����û���Ϣ" << endl;
    /*char buffer[200];
    bzero(&buffer, strlen(buffer));
    res = recv(sock, buffer, 200, 0);
    string userOnline(&buffer[0], &buffer[strlen(buffer)]);
    cout << userOnline << endl;*/
    cout << "������ȡ�û���Ϣ"<<endl;
    bzero(&buff, strlen(buff));

   
    res = recv(sock, buff, 200, 0);
    
    string userName(&buff[0], &buff[strlen(buff)]);
    cout << userName << endl;
   
    cout << "��ѡ����Ҫͨ������" << endl;
    userName="";
    cin >> userName;
    send(sock, NULL, 0, 0);

    chatToSomeoneTool(userName);

    
}

int client::function1()
{
    using namespace std;
    int choice1;
    system("clear");
    cout << "=================================================" << endl;
    cout << "=              Welcome to menu!                 =" << endl;
    cout << "=================================================" << endl;
    cout << "=                                               =" << endl;
    cout << "=  1.Ⱥ��  2.˽��  3.�ļ��ϴ�  4.�ļ�����  5.�˳� =" << endl;
    cout << "=                                               =" << endl;
    cout << "=================================================" << endl;
    cout << "Your Choice >: ";
    func = 2;
    return 0;
}



std::string client::sendmsg()
{
   
        int res = send(sock, (msg.c_str()), msg.size() + 1, 0);
        //cout<<"res = "<<res<<endl;
        msgrev="";
        msgrev.resize(10);
        recv(sock, (void*)msgrev.c_str(), msgrev.size(), 0);
        
        return msgrev;
    
}
//˽�ĵĹ���
void client::chatToSomeoneTool(string& username)
{

    //�ܵ�
    int pipe_fd[2];
    if (pipe(pipe_fd) < 0) {
        perror("pipe error");
        exit(-1);
    }


    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd < 0) { perror("epfd error"); exit(-1); }
    static struct epoll_event events[2];
    //��sock�͹ܵ����˶��ӵ��ں��¼�����
    addfd(epfd, sock, true);
    addfd(epfd, pipe_fd[0], true);

    // ��ʾ�ͻ����Ƿ���������
    bool isClientwork = true;

    // ������Ϣ������
    string msg;

    //Fork
    int pid = fork();
    if (pid < 0) { perror("fork error"); exit(-1); }
    else if (pid == 0) {

        close(pipe_fd[0]);
        printf("Please input 'exit' to exit the chat\n");        
        while (isClientwork) {
        

        getline(cin, msg);

        //�ͻ������exit���˳�
        if (msg == "exit") {
            isClientwork = 0;
            return;
          
        }
        else {
            string sendMsg;
            sendMsg = "4," + username + "," + msg+"," + myUserName;
            if (write(sock, sendMsg.c_str(), sendMsg.size()) < 0) {
                { perror("fork error"); exit(-1); }
            }

        }
    }
    }
    else {
        close(pipe_fd[1]);

        while (isClientwork) {
            int epoll_events_count = epoll_wait(epfd, events, 2, -1);
            //��������¼�
            for (int i = 0; i < epoll_events_count; i++) {
                
                //����˷�����Ϣ
                if (events[i].data.fd == sock) {
                    //���ܷ������Ϣ
                    char msgRev[1024];
                    int ret = recv(sock,msgRev, 1024, 0);
                    string printMsg(&msgRev[0], &msgRev[strlen(msgRev)]);
                    //ret = 0  ����˹ر�
                    if (ret == 0) {
                        printf("Server closed connection: %d\n", sock);
                        close(sock);
                        isClientwork = 0;
                    }
                    else {
                        cout << printMsg<<endl;
                    }
                }
                else {
                    string msgSend;
                    //�ӽ���д���¼������������̴�����������
                    int ret = read(events[i].data.fd, (void*)msgSend.c_str(), 200);
                    if (ret = 0) {
                        isClientwork = 0;
                    }
                    else {
                        send(sock, msgSend.c_str(), 200, 0);
                    }
                }
            }
        }

    }

    if (pid) {
        close(pipe_fd[1]);
        close(sock);
    }
    else {
        close(pipe_fd[0]);
    }

   



    cout << "chat end" << endl;
  
}

void client::getinfor()
{
    std::string user, passwd;

    std::cout << "�û�����";
    std::cin >> user;
    msg += user;
    std::cout << "��  �룺";
    std::cin >> passwd;
    //msg = msg + " "+passwd;
    msg = msg + "," + passwd;
    //cout<<"send msg = "<<msg<<endl;
    sendmsg();
    if (strncmp("1", msgrev.c_str(), 1) == 0 || strncmp("register success!", msgrev.c_str(), 10) == 0)
        {
            
            myUserName = user;
            login = true;
           
        }
    else
        {
        
          std::cout << "�û������������" << std::endl;
          sleep(1);
          system("clear");
            //        //cout<<msg.c_str()<<endl;
                
          }

}

//Ⱥ��
void client::chat()
{
        int pipe_fd[2];
        if (pipe(pipe_fd) < 0) {
            perror("pipe error");
            exit(-1);
        }

      
        int epfd = epoll_create(EPOLL_SIZE);
        if (epfd < 0) { perror("epfd error"); exit(-1); }
        static struct epoll_event events[2];
        //��sock�͹ܵ����˶��ӵ��ں��¼�����
        addfd(epfd, sock, true);
        addfd(epfd, pipe_fd[0], true);

        // ��ʾ�ͻ����Ƿ���������
        bool isClientwork = true;

        // ������Ϣ������
        char message[BUF_SIZE];


        //Fork
        int pid = fork();
        if (pid < 0) { perror("fork error"); exit(-1); }
        else if (pid == 0) {  
            
            close(pipe_fd[0]);
            printf("Please input 'exit' to exit the chat\n");

            while (isClientwork) {
                bzero(&message, BUF_SIZE);
                fgets(message, BUF_SIZE, stdin);

                //�ͻ������exit���˳�
                if (strncasecmp(message, EXIT, strlen(EXIT)) == 0) {
                    string a = "3?";
                    write(pipe_fd[1], a.c_str(), a.size());
                       
                    isClientwork = 0;
                   
                   
                }
                else {
                    int strLen = 20;
                    //�ӽ��̽���Ϣд��ܵ�
                    //char* dest = (char*)malloc(strLen * sizeof(char));
                    //dest[1] = '\0';//����ǹؼ�
                    //dest[0] = '3';
                    //strcat(dest, message);
                  
                    string dest = "3";
                    dest += message;
                    
                    if (message[1]!=0) {
                        if (write(pipe_fd[1], dest.c_str(), dest.size()) < 0) {
                            { perror("fork error"); exit(-1); }
                        }
                    }
                }
            }
        }
        else {	
            close(pipe_fd[1]);

            while (isClientwork) {
                int epoll_events_count = epoll_wait(epfd, events, 2, -1);
                //��������¼�
                for (int i = 0; i < epoll_events_count; i++) {
                    bzero(&message, BUF_SIZE);
                    //����˷�����Ϣ
                    if (events[i].data.fd == sock) {
                        //���ܷ������Ϣ
                        int ret = recv(sock, message, BUF_SIZE, 0);
         
                        //ret = 0  ����˹ر�
                        if (ret == 0) {
                            printf("Server closed connection: %d\n", sock);
                            close(sock);
                            isClientwork = 0;
                        }
                        else {
                            printf("%s\n", message);
                        }
                    }
                    else {
                        //�ӽ���д���¼������������̴�����������
                        int ret = read(events[i].data.fd, message, BUF_SIZE);
                        if (ret = 0) {
                            isClientwork = 0;
                        }
                        else {
                            send(sock, message, BUF_SIZE, 0);
                        }
                    }
                }
            }

        }

        if (pid) {
            close(pipe_fd[1]);
            
        }
        else {
            close(pipe_fd[0]);
        }
       


        
        cout << "chat end"<<endl;
    
}


void client::do_work()
{
    if (!connRedis()) {
        return;
    };
    login = false;
    while (!login) {
        wellcome();
        string select;
        std::cin >> select;
        while (select[0]-'0' > 2 || select[0] - '0' < 1||select.size()>1) {
            cout << "��������ȷ����" << endl;
            cin >> select;
        }
        switch (select[0] - '0') {
        case 1: //��¼
            msg = "1,";
            getinfor();
            break;
        case 2:
            msg = "2,";
            logIn();
            break;
        default:
            cout << "��������ȷ����" << endl;

            break;
        }         
    }
    while (login) {
        function1();
        string select;
        std::cin >> select;
        while (select[0] - '0' > 5 || select[0] - '0' < 1||select.size() > 1) {
            cout << "��������ȷ����" << endl;
            cin >> select;
        }
        switch (select[0] - '0') {
        case 1:
            msg = "3,";
            send(sock, NULL, 0, 0);
            chat();
            break;
        case 2:
            msg = "4,";
            chatToSomeone();
            break;
        case 3:
            sendFile();
            sleep(1);
            break;
        case 4:
            msg = "5";
            downloadFile();
            sleep(1);
            break;
        case 5:
            
            login = false;
            cout << "�˳��ͻ���" << endl;
            break;
        default:
            cout << "��������ȷ����" << endl;

            break;
        }

    }
    
}

void client::sendFile()
{
    getFileExists(filenames);
    cout << "���ϴ��ļ�" << endl;
    for (auto iter = filenames.begin(); iter < filenames.end(); iter++) {
        cout << *iter << endl;
    }
    cout << "���������ϴ��ļ�"<<endl;
    string fileName;
    cin >> fileName;
    bool fileExits;
    for (int i = 0; i < filenames.size(); i++) {
        if (fileName == filenames[i]) {
            fileExits = true; break;
        }

    }
    //û����
    //�ļ�����
    if (fileExits) {
        FILE* fp;

        string path = "./";
        path = path+ fileName;
        /*if ((fp = fopen(path.c_str(), "r")) == NULL) {
            printf("File open.\n");
        }
        char  buffer[4096];       
        bzero(buffer, sizeof(buffer));
        char dest[2] = { '6',',' };
        int len = fread(buffer, 1, sizeof(buffer), fp);
        fileName += ",";
        strcat(dest, fileName.c_str());
        strcat(dest, buffer);
        int t = send(sock, dest, 1024, 0);
        fclose(fp);
        fp = NULL;*/
        int fd = open(path.c_str(), O_RDWR);
        if (fd == -1)
        {
            printf("open  failed");
            return;
        }
        char  buff[4096];
        bzero(buff, sizeof(buff));
        string dest="6,";
      
        //��ȡ����
        int ret = read(fd, buff, sizeof(buff));

        fileName += ",";
        dest += fileName;
        dest += buff;
        int t = send(sock, dest.c_str(), dest.size(), 0);
        close(fd);
        cout << "�ϴ��ɹ�" << endl;
        return;
       
    }
    else {
        cout << "�ļ�������"<<endl;
        
        return;
     
    }
    

}

void client::getFileExists(vector<string>& filenames)
{
    filenames.clear();
    DIR* pDir;
    if (!(pDir = opendir("./"))) {

        cout << "Folder doesn't Exist!" << endl;
        return;
    }
    dirent* ptr;
    while ((ptr = readdir(pDir)) != NULL) {
        if (strncmp(ptr->d_name, ".",1) != 0 && strncmp(ptr->d_name, "..",2) != 0) {
            filenames.push_back(string(ptr->d_name));
        }
    }
    closedir(pDir);
}






//bool client::SendFile(string filename) {
//	string a = "../resources/" + filename;
//	char* p = const_cast<char*>(a.c_str());
//	if (stat(p, &buf) != 0) {
//		cout << "file not exist";
//		return false;
//	}
//	else{
//		ofstream out;
//		out.open(a);
//		stringstream res;
//		res<<out.rdbuf();
//		out.close();
//
//	}
//}
//


