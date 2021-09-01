#pragma once

#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <semaphore.h>
#include <thread>
#ifndef SQLCONNPOOL_H
#define SQLCONNPOOL_H
class pool {
public:
    static pool* Instance()
    {
        static pool tmp;
        return &tmp;
    }

    MYSQL* GetConn();
    void FreeConn(MYSQL* conn);
    int GetFreeConnCount();

    void Init(const char* host, int port,
        const char* user, const char* pwd,
        const char* dbName, int connSize);
    void ClosePool();

private:
    pool();
    ~pool();

    int MAX_CONN_;
    int useCount_;
    int freeCount_;
    std::queue<MYSQL*> connQue_;
    std::mutex mtx_;
    sem_t semId_;
};
#endif