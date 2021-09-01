#pragma once
#ifndef SQLCONNRAII_H
#define SQLCONNRAII_H
#include "pool.h"
#include <cassert>
//������Ҫע����ǣ��ڻ�ȡ����ʱ��ͨ���вι���Դ���Ĳ��������޸ġ��������ݿ����ӱ�����ָ�����ͣ����Բ�����Ҫͨ��˫ָ����ܶ�������޸�
/* ��Դ�ڶ������ʼ�� ��Դ�ڶ�������ʱ�ͷ�*/
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