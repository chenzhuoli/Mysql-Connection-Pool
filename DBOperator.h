#ifndef DBOPERATOR_H
#define DBOPERATOR_H

#include <iostream>
#include <string>
#include <mysql.h>
#include <vector>
using std::vector;
using std::string;

class DBOperator{
    public:
        DBOperator();
        ~DBOperator();
        void DBCoInit();
        bool DBInit(const std::string& _host,const std::string& _username,const std::string& _password,const std::string& _dbname,const int& _port);
        bool Query(const std::string& sql,vector<std::string>&keyList,vector<vector<std::string> > &valueList);
        bool Modify(const std::string& sql);
        
    protected:
        void setHost(const std::string& _host);
        void setUsername(const std::string& _username);
        void setPassword(const std::string& _password);
        void setDbname(const std::string& _dbname);
        void setPort(const int& _port);
        
    private:
        bool isconnected;
        MYSQL mysqlclient;
        int port;
        char host[256];
        char username[256];
        char password[256];
        char dbname[256];
    
    
};


#endif
