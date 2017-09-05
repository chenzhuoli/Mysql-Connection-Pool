
#ifndef _MYSQL_CONN_POOL_H_
#define _MYSQL_CONN_POOL_H_

#include <queue>
#include <pthread.h>
#include<stdexcept>
#include "DBOperator.h"

using namespace std;

class MySqlException:public exception {
  string what_str;   // Error description
public:
  MySqlException(const string &what_arg) throw():what_str(what_arg) { }
  ~MySqlException() throw(){}
  virtual const char *what() const throw() {
    return what_str.c_str();
  }
};

class MysqlConnPool 
{
public:
	//MysqlConnPool() throw(MySqlException);
	MysqlConnPool() throw();
	~MysqlConnPool();

	// 设置数据库的地址
	void setMysqlConfig(const string& ip, const string& user, const string& passwd, const string& db_name, const int& port);

	// 初始化数据库连接池
	//void initConnPool(u_int size) throw(MySqlException);
	void initConnPool(u_int size) throw();

	// 从连接池中取出一个连接
	DBOperator* get();

	// 向连接池放回一个连接
	void put(DBOperator* conn);

private:
	DBOperator*  pMysqlArray;
	queue<DBOperator*> mysqlQueue;
	pthread_mutex_t mQLock;
	pthread_cond_t mQCond;
	u_int mSize;

	string		mHost;
	int 		mPort;
	string		mUser;
	string		mPasswd;
	string		mDb;
};

#endif
