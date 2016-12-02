#include <stdlib.h>
#include <stdio.h>
#include <errmsg.h>
#include "DBOperator.h"
DBOperator::DBOperator()
{
    isconnected = false;
    memset(host,'\0',sizeof(host));
    memset(username,'\0',sizeof(username));
    memset(password,'\0',sizeof(password));
    memset(dbname,'\0',sizeof(dbname));
}

void DBOperator::DBCoInit()
{
    if(isconnected)
    {
        mysql_close(&mysqlclient);
        mysql_library_end();
        isconnected = false;
    }
}

DBOperator::~DBOperator()
{
    DBCoInit();
}

void DBOperator::setDbname(const std::string& _dbname)
{
    if ( !_dbname.empty() )
    {
        strcpy(this->dbname,_dbname.c_str());
    }
}

void DBOperator::setHost(const std::string& _host)
{
    if ( !_host.empty() )
    {
        strcpy(this->host,_host.c_str());
    }
}

void DBOperator::setUsername(const std::string& _username)
{
    if ( !_username.empty() )
    {
        strcpy(this->username,_username.c_str());
    }
}

void DBOperator::setPassword(const std::string& _password)
{
    if ( !_password.empty() )
    {
        strcpy(this->password,_password.c_str());
    }
}

void DBOperator::setPort(const int& _port)
{
    this->port = _port;
}

bool DBOperator::DBInit(const std::string& _host,const std::string& _username,const std::string& _password,const std::string& _dbname,const int& _port)
{
    //若database已经连接好，直接返回
    if (isconnected)
    {
        return true;
    }
 
    this->setHost(_host);
    this->setUsername(_username);
    this->setPassword(_password);
    this->setDbname(_dbname);
    this->setPort(_port);
                             
    mysql_init(&mysqlclient);
    int mysql_query_errno;
    //正常的mysql连接,尝试连接一次就好
    mysql_options(&mysqlclient,MYSQL_SET_CHARSET_NAME,"latin1");
    if (!mysql_real_connect(&mysqlclient ,host,username,password,dbname,port,NULL,0))
    {
        mysql_query_errno = mysql_errno(&mysqlclient);
        fprintf(stderr,"database connection failure,errno=%d~~!\n",mysql_query_errno);
        isconnected = false;
        return false;
    }
    else
    {
        mysql_query(&mysqlclient,"set names gbk");
        isconnected = true;
        return true;
    }
}

bool DBOperator::Query(const std::string& sql,vector<std::string>&keyList,vector<vector<std::string> > &valueList)
{
    int i = mysql_real_query(&mysqlclient,sql.c_str(),sql.size());
    
    //若mysql断线重连mysql
    if ( i != 0 )
    {
        isconnected = false;
        DBInit(host,username,password,dbname,port);
        //重新连接完毕,若重连仍然失败，则返回false
        if (!isconnected)
        {
            fprintf(stderr,"database connection failure~~\n");
            return false;
        }

        //重新连接好后，再次请求query值
        mysql_real_query(&mysqlclient,sql.c_str(),sql.size());
    }

    MYSQL_RES *res;
    MYSQL_FIELD *fd;

    res = mysql_store_result(&mysqlclient);
    for( ;fd=mysql_fetch_field(res); )
    {
        keyList.push_back(fd->name);
    }
    
    vector<string> objectValue;
    MYSQL_ROW row;
    while( (row = mysql_fetch_row(res)))
    {
        //objectValue.clear();
        for(unsigned int j=0 ; j<mysql_num_fields(res) ; j++)
        {
            
            if(row[j])
                objectValue.push_back(row[j]);
            else
                objectValue.push_back("");
        }
        valueList.push_back(objectValue);
        objectValue.clear();
    }
    
    mysql_free_result(res);
    return true;
}

bool DBOperator::Modify(const std::string& sql)
{
    //若database没有连接，则重新连接
    if ( !isconnected )
    {
        if(!DBInit(host,username,password,dbname,port))
        {
            fprintf(stderr,"do not connect to database~~!\n");
            return false;//若重新连接失败，则返回false
        }
        else//若重连成功，则isconnected状态改变为true
        {
            isconnected = true;
        }
    }
    if( mysql_query(&mysqlclient,sql.c_str()))
    {
        fprintf(stderr,"%s\n",mysql_error(&mysqlclient));
        return false;
    }
    else
    {
        return true;
    }
}

#ifdef DBOPERATOR_UNIT_TEST
/*
int main()
{
    string host=string("192.168.85.132");
    string username=string("root");
    string password=string("root");
    string dbname=string("search_v3_test");

    int port=3306;
    DBOperator db_op;
    db_op.DBInit(host,username,password,dbname,port);
    vector<string> keyList;
    vector<vector<string> > valueList;
    string sql=string("select * from b2c_shop;");
    db_op.Query(sql,keyList,valueList);
    std::cout<<keyList.size()<<endl;
    cout<<valueList.size()<<endl;
    return 0;    
}
*/
#endif


