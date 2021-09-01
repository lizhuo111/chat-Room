#pragma once
#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H
#include "pool.h"
#include <cassert>
//这里需要注意的是，在获取连接时，通过有参构造对传入的参数进行修改。其中数据库连接本身是指针类型，所以参数需要通过双指针才能对其进行修改
/* 资源在对象构造初始化 资源在对象析构时释放*/
class SqlConnRAII {
public:
    SqlConnRAII(MYSQL** sql, pool* connpool) {
        assert(connpool);
        *sql = connpool->GetConn();
        sql_ = *sql;
        connpool_ = connpool;
    }

    ~SqlConnRAII() {
        if (sql_) { connpool_->FreeConn(sql_); }
    }

private:
    MYSQL* sql_;
    pool* connpool_;
};

#endif //SQLCONNRAII_H