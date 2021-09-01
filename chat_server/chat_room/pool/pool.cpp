#include "pool.h"
#include <cassert>
#include<iostream>
#include"mysql/mysql.h"
using namespace std;

MYSQL *pool::GetConn()
{
	MYSQL* sql = nullptr;
	if (connQue_.empty()) {
		return nullptr;
	}
	sem_wait(&semId_);  
	if (!connQue_.empty()) {
		lock_guard<mutex> locker(mtx_);
		sql = connQue_.front();
		connQue_.pop();

		cout << connQue_.size()<<endl;

	}
	return sql;
}

void pool::FreeConn(MYSQL* conn)
{
	assert(conn);
	lock_guard<mutex> locker(mtx_);
	connQue_.push(conn);
	sem_post(&semId_);   //信号量+1 
}

int pool::GetFreeConnCount()
{
	lock_guard<mutex> locker(mtx_);
	return connQue_.size();
}

void pool::Init(const char* host, int port, const char* user, const char* pwd, const char* dbName, int connSize=10)
{
	assert(connSize>0);
	for (int i = 0; i < connSize; i++) {
		MYSQL* sql = nullptr;
		sql = mysql_init(sql);
		if (!sql) { std::cout << "err"; return; }
		sql=mysql_real_connect(sql, host, user, pwd, dbName, port, nullptr, 0);
		if (!sql) { std::cout << "err"; return; }
		connQue_.push(sql);
	}
	MAX_CONN_ = connSize;
	sem_init(&semId_, 0, MAX_CONN_);  //信号量


}

void pool::ClosePool()
{
	lock_guard<mutex> locker(mtx_);
	while (!connQue_.empty()) {
		auto item = connQue_.front();
		connQue_.pop();
		mysql_close(item);
	}
	mysql_library_end();           //释放内存
}

pool::pool()
{
	useCount_ = 0;
	freeCount_=0;
}

pool::~pool()
{
	ClosePool();
}


