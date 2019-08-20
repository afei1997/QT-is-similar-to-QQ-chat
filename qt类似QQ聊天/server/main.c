#include "datamessage.h"
#include "clientresult.h"
#include <stdio.h>
#include <sqlite3.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

sqlite3 *db=NULL;
int sockfd_init();
static void client_connect(void *arc);

int main()
{
    int sockfd=sockfd_init();
    if(-1==sockfd)
        return -1;

    db=db_create();

    struct sockaddr_in caddr;
    bzero(&caddr,sizeof(caddr));
    socklen_t clen=sizeof(caddr);
    char cip[32]={0};
    char cport[16]={0};
    int ret=-1;
    while (1)
    {
        bzero(&caddr,sizeof(caddr));
        int rws=accept(sockfd,(struct sockaddr *)&caddr,&clen);
        if(-1==rws)
        {
            perror("accept");
            break;
        }
        bzero(cip,sizeof(cip));
        bzero(cport,sizeof(cport));
        pthread_t t;
        ret=pthread_create(&t,NULL,(void *)&client_connect,(void *)&rws);
        if(-1==ret)
        {
            perror("pthread_create");
            continue;
        }
        pthread_detach(t);
    }
    db_close(db);
    return 0;
}

int sockfd_init()
{
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==fd)
    {
        perror("sockfd");
        return -1;
    }
//    printf("sockfd=%d\n",fd);

    int n=1;
    int ret=setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&n,sizeof(n));
    if(-1==ret)
    {
        perror("setsockopt");
        close(fd);
        return -1;
    }
    struct sockaddr_in saddr;
    bzero(&saddr,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(8888);
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);
    socklen_t slen=sizeof(saddr);
    ret=bind(fd,(struct sockaddr *)&saddr,slen);
    if(-1==ret)
    {
        perror("bind");
        close(fd);
        return -1;
    }
    printf("bind success\n");

    ret=listen(fd,1024);
    if(-1==ret)
    {
        perror("listen");
        close(fd);
        return -1;
    }
    printf("listen....\n");
    return fd;
}

static void client_connect(void *arc)
{
    printf("连接成功\n");
    int sockfd=*((int *)arc);
    char buf[1024]={0};
    int ret;
    char id[20]={0};
    char pass[50]={0};
    int flag=0;
    while (1)
    {
        bzero(buf,sizeof(buf));
        ret=read(sockfd,buf,sizeof(buf));
        if(-1==ret)
        {
            perror("read one");
            close(sockfd);
            pthread_exit(NULL);
        }
        else if (0==ret)
        {
            printf("客户端退出\n");
            update_online_db(db,sockfd);
            if(1==flag)
                client_exit(db,id);
            close(sockfd);
            pthread_exit(NULL);
        }
        int n=buf[0]-'0';
        printf("%s\n",buf);
        switch (n)
        {
            case 0:
                ret=sign_in(db,buf+2,sockfd);
                if(-1==ret || 1==ret)
                    break;
                else
                {  
                    bzero(id,sizeof(id));
                    bzero(pass,sizeof(pass));
                    sscanf(buf+2,"%s%s",id,pass);
                    flag=1;
                }
                break;
            case 1:
                ret=logon(db,buf+2,sockfd);
                if(-1==ret || 1==ret)
                    break;
                else
                {
                    printf("注册退出\n");
                    close(sockfd);
                    pthread_exit(NULL);
                }
            case 6:
                ret=send_other(db,buf,sockfd);   
                break;
            default:
                if(1==flag)
                client_exit(db,id);
                update_online_db(db,sockfd);
                printf("客户端退出\n");
                close(sockfd);
                pthread_exit(NULL);
                break;
        }
    }
    if(1==flag)
        client_exit(db,id);
    update_online_db(db,sockfd);
    printf("客户端退出\n");
    close(sockfd);
    pthread_exit(NULL);
}