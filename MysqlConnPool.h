
/*
 * =====================================================================================
 *
 *       Filename:  mysql_pool.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014-4-13
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  liujie (), liujiejs01@dangdang.com
 *        Company:  dangdang.com
 *
 * =====================================================================================
 */
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

	// �������ݿ�ĵ�ַ
	void setMysqlConfig(const string& ip, const string& user, const string& passwd, const string& db_name, const int& port);

	// ��ʼ�����ݿ����ӳ�
	//void initConnPool(u_int size) throw(MySqlException);
	void initConnPool(u_int size) throw();

	// �����ӳ���ȡ��һ������
	DBOperator* get();

	// �����ӳطŻ�һ������
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
