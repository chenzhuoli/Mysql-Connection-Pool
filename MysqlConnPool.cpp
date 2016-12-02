/*
 * =====================================================================================
 *
 *       Filename:  mysql_pool.cpp
 *
 *    Description:  Á¬½Ó³Ø
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
#include "MysqlConnPool.h"

MysqlConnPool::MysqlConnPool() throw():
	pMysqlArray(NULL),
	mSize(0),
	mHost(""),
	mPort(0),
	mUser(""),
	mPasswd(""),
	mDb("")
{
	if(pthread_mutex_init(&mQLock,NULL) != 0)
		throw MySqlException("MysqlConnPool::initConnPool: init mutex failed!");

	if(pthread_cond_init(&mQCond,NULL) != 0)
		throw MySqlException("MysqlConnPool::initConnPool: init cond failed!");
}

MysqlConnPool::~MysqlConnPool()
{
	if(pMysqlArray)
		delete [] pMysqlArray;	
	
	pthread_mutex_destroy(&mQLock);
	pthread_cond_destroy(&mQCond);
}

void MysqlConnPool::setMysqlConfig(const string& ip, const string& user, const string& passwd, const string& db_name, const int& port)
{
	mHost = ip;
	mUser = user;
	mPasswd = passwd;
	mDb = db_name;
	mPort = port;
}

void MysqlConnPool::initConnPool(u_int size) throw()
{
	mSize = size;
	pMysqlArray = new DBOperator[size];
	if(!pMysqlArray)
		throw MySqlException("MysqlConnPool::initConnPool: allocate mysql array failed!");

	try {
		for(u_int i=0; i<size; i++) {
			pMysqlArray[i].DBInit(mHost, mUser, mPasswd, mDb, mPort);
			mysqlQueue.push(&pMysqlArray[i]);
		}
	}catch (MySqlException & e) {
		throw e;
	}
}

DBOperator* MysqlConnPool::get()
{
	pthread_mutex_lock(&mQLock);
	while(mysqlQueue.empty())
		pthread_cond_wait(&mQCond, &mQLock);
	DBOperator* mysql = mysqlQueue.front();
	mysqlQueue.pop();
	pthread_mutex_unlock(&mQLock);

	return mysql;
}

void MysqlConnPool::put(DBOperator* conn)
{
	pthread_mutex_lock(&mQLock);
	mysqlQueue.push(conn);
	pthread_mutex_unlock(&mQLock);
	pthread_cond_signal(&mQCond);
}
