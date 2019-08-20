#include "datamessage.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

sqlite3 *db_create(void)
{
    sqlite3 *db=NULL;
    int ret=sqlite3_open("my.db",&db);
    if(-1==ret)
    {
        printf("db_create error :%s\n", sqlite3_errmsg(db));
        return NULL;
    }
    char *p="CREATE TABLE IF NOT EXISTS user(id INTEGER PRIMARY KEY NOT NULL,username VARCHAR(50) NOT NULL,userpass VARCHAR(50) NOT NULL,usericon VARCHAR(50))";
    char *errmsg;
    ret=sqlite3_exec(db, p, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("user 184:%s\n",errmsg);
        sqlite3_close(db);
        return NULL;
    }
    char *q="CREATE TABLE IF NOT EXISTS userfriend(aid INTEGER NOT NULL,bid INTEGER NOT NULL)";
    ret=sqlite3_exec(db, q, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("user 184:%s\n",errmsg);
        sqlite3_close(db);
        return NULL;
    }
    char *k="CREATE TEMPORARY TABLE online(id INTEGER PRIMARY KEY NOT NULL,isonline INTEGER NOT NULL DEFAULT 0,sockfd INTEGER DEFAULT -1)";
    ret=sqlite3_exec(db, k, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("user:%s\n",errmsg);
        sqlite3_close(db);
        return NULL;
    }

    ret=sqlite3_exec(db, "INSERT INTO online(id) SELECT id FROM user", NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("user:%s\n",errmsg);
        sqlite3_close(db);
        return NULL;
    }
    return db;
}

int sign_in_db(sqlite3 *db,int id,char *userpass,char *username,char *usericon,int sockfd)
{
    if(NULL==db || id < 10000000)
        return -1;
    char chmond[512]={0};
    sprintf(chmond,"SELECT * FROM user WHERE id=%d AND userpass='%s'",id,userpass);
    int n,m;
    char **pazresult=NULL,*errmsg;
    int ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(0==n)
        return -1;

    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"SELECT * FROM online WHERE id=%d AND isonline=1",id);
    int i,j;
    char **pazresult1=NULL;
    ret=sqlite3_get_table(db,chmond,&pazresult1,&i,&j,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    } 
    if(1==i)
        return 1;

    strcpy(username,pazresult[m+1]);
    strcpy(usericon,pazresult[m+3]);
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"UPDATE online SET isonline=1,sockfd=%d WHERE id=%d",sockfd,id);
    ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("online sockfd:%s\n",errmsg);
        return -1;
    }
    return 0;  
}

int logon_db(sqlite3 *db,int id,char *username,char *userpass)
{
    if(NULL==db || id < 10000000)
        return -1;
    char chmond[512]={0};
    sprintf(chmond,"SELECT * FROM user WHERE id=%d",id);
    int n,m;
    char **pazresult=NULL,*errmsg;
    int ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(1==n)
        return n;
    else
    {
        bzero(chmond,sizeof(chmond));
        sprintf(chmond,"INSERT INTO user VALUES(%d,'%s','%s','image/1.jpg')",id,username,userpass);
        int ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
        if(ret!=SQLITE_OK)
        {
            printf("logon_db:%s\n",errmsg);
            return -1;
        }
        return 0;
    }
}

int find_friend_db(sqlite3 *db,int Aid,int *n,int *m,char ***pazresult)
{
    if(NULL==db || Aid < 10000000 )
        return -1;
    char chmond[512]={0};
    sprintf(chmond,"SELECT * FROM user WHERE id=%d",Aid);
    char *errmsg;
    int ret=sqlite3_get_table(db,chmond,pazresult,n,m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("find_friend_db 1 %s\n",errmsg);
        return -1;
    }
    if(0==*n)
        return -1;
    
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"SELECT u.id,u.username,o.isonline,u.usericon FROM user AS u INNER JOIN online AS o ON u.id=o.id AND u.id IN (SELECT Bid FROM userfriend WHERE Aid=%d)",Aid);
    ret=sqlite3_get_table(db,chmond,pazresult,n,m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("find_friend_db 2 %s\n",errmsg);
        return -1;
    }
    if(0==*n)
        return 1;
    return 0;
}

int add_friend_db(sqlite3 *db,int Aid,int Bid)
{
    if(NULL==db || Aid < 10000000 || Bid< 10000000)
        return -1;
    char chmond[512]={0};
    sprintf(chmond,"SELECT * FROM user WHERE id=%d OR id=%d",Aid,Bid);
    int n,m;
    char **pazresult=NULL,*errmsg;
    int ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(1==n)
        return -1;
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"SELECT * FROM userfriend WHERE aid=%d AND Bid=%d",Aid,Bid);
    ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(1==n)
        return n;

    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"INSERT INTO userfriend VALUES(%d,%d)",Aid,Bid);
    ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("add 1:%s\n",errmsg);
        return -1;
    }
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"INSERT INTO userfriend VALUES(%d,%d)",Bid,Aid);
    ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("add 2:%s\n",errmsg);
        return -1;
    }
    return 0;
}

int delete_friend_db(sqlite3 *db,int Aid,int Bid)
{
    if(NULL==db || Aid < 10000000 || Bid< 10000000)
        return -1;
    char chmond[512]={0};
    int n,m;
    char **pazresult=NULL,*errmsg;
    sprintf(chmond,"SELECT * FROM userfriend WHERE aid=%d AND Bid=%d",Aid,Bid);
    int ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(0==n)
        return 1;
    
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"DELETE FROM userfriend WHERE Aid=%d AND Bid=%d",Aid,Bid);
    
    ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    bzero(chmond,sizeof(chmond));
    sprintf(chmond,"DELETE FROM userfriend WHERE Aid=%d AND Bid=%d",Bid,Aid);
    ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    return 0;
}

int update_online_db(sqlite3 *db,int sockfd)
{
    if(NULL==db)
        return -1;
    char chmond[512]={0};
    char *errmsg;
    sprintf(chmond,"UPDATE online SET isonline=0,sockfd=-1 WHERE sockfd=%d",sockfd);
    int ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("update_online_db:%s\n",errmsg);
        return -1;
    }
    return 0;
}

int update_user_db(sqlite3 *db,int id,char *username,char *usericon)
{
    if(NULL==db || id < 10000000)
        return -1;
    char chmond[512]={0};
    if(NULL == username && NULL != usericon)
    {
        sprintf(chmond,"UPDATE user SET usericon='%s' WHERE id=%d",usericon,id);
    }
    else if (NULL != username && NULL == usericon)
    {
        sprintf(chmond,"UPDATE user SET username='%s'WHERE id=%d",username,id);
    }
    else
    {
        sprintf(chmond,"UPDATE user SET username='%s',usericon='%s' WHERE id=%d",username,usericon,id);
    }
    char *errmsg;
    int ret=sqlite3_exec(db, chmond, NULL,NULL, &errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("update_user_db:%s\n",errmsg);
        return -1;
    }
    return 0;
}

int db_close(sqlite3 *db)
{
    if(NULL==db)
        return -1;
    sqlite3_close(db);
    return 0;
}

int find_sockfd_db(sqlite3* db,int id,int *sockfd)
{
    if(NULL==db || id < 10000000)
        return -1;
    char chmond[512]={0};
    sprintf(chmond,"SELECT sockfd FROM online WHERE id=%d AND isonline=1",id);
    int n,m;
    char **pazresult=NULL,*errmsg;
    int ret=sqlite3_get_table(db,chmond,&pazresult,&n,&m,&errmsg);
    if(ret!=SQLITE_OK)
    {
        printf("errmsg=%s\n",errmsg);
        return -1;
    }
    if(0==n)
        return 1;
    else
    {
        *sockfd=atoi(pazresult[m]);
        return 0;
    }
}
