#include "clientresult.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#define MAXSIZE 1024
sqlite3 *db1=NULL;
static void send_icon(void *arc);

int sign_in(sqlite3 *db,char *chmond,int sockfd)
{
    db1=db;
    char id[20]={0};
    char pass[50]={0};
    sscanf(chmond,"%s%s",id,pass);
    int userid=atoi(id);
    char name[50]={0};
    char icon[50]={0};
    int ret=sign_in_db(db,userid,pass,name,icon,sockfd);
    if(-1==ret)
    {
        write(sockfd,"no",MAXSIZE);
        return -1;
    }
    else if (1==ret)
    {
        write(sockfd,"online",MAXSIZE);
        return 1;
    }
    else
    {
        write(sockfd,"yes",5);
        usleep(10000);
        char buf[MAXSIZE]={0};
        write(sockfd,name,MAXSIZE);//发送客户端用户名
        usleep(10000);

        struct stat info;
		if(0 > stat(icon, &info)){
			perror("stat");
			return -1;
		}
		int len = info.st_size;

		int size = htonl(len);
		if(4 != write(sockfd, &size, 4))
        {
			// printf("write head fail.\n");
			return -1;;
		}
		printf("snd file size %dbytes done.\n", len);

		FILE *fp = fopen(icon, "r");
		if(NULL == fp){
			perror("fopen");
			return -1;
		}

		while(1){
            bzero(buf,sizeof(buf));
			size = fread(buf, 1, MAXSIZE, fp);
			if(0 == size){
				break;
			}
			// printf("%s: read file : size = %d\n", icon, size);

			char *p = buf;
			len = size;
			while(len){
				size = send(sockfd, p, len, MSG_NOSIGNAL);
				if(0 >= size){
					printf("write \"%s\" fail.\n", icon);
					return -1;
				}
				p  += size;
				len-= size;

				// printf("%s: size = %d\n", icon, size);
			}
		}
		fclose(fp);
        usleep(10000);
        pthread_t t;
        bzero(buf,sizeof(buf));
        sprintf(buf,"0 %d %d",userid,sockfd);
        ret=pthread_create(&t,NULL,(void *)&send_icon,(void *)buf);
        pthread_detach(t);
        return 0;
    }
}

int logon(sqlite3 *db,char *chmond,int sockfd)
{
    char id[20]={0};
    char name[50]={0};
    char pass[50]={0};
    sscanf(chmond,"%s%s%s",id,name,pass);
    int userid=atoi(id);
    int ret=logon_db(db,userid,name,pass);
    if(-1==ret)
    {
        write(sockfd,"no",MAXSIZE);
        return -1;
    }
    else if (1==ret)
    {
        write(sockfd,"existing",MAXSIZE);
        return 1;
    }
    else
    {
        write(sockfd,"yes",MAXSIZE);
        return 0;
    }
}

int update_user(sqlite3 *db,char *chmond,int n,int sockfd)
{
    if(0==n)
    {
        char aid[50]={0};
        char name[50]={0};
        sscanf(chmond,"%s%s",aid,name);
        int id=atoi(aid);
        int ret=update_user_db(db,id,name,NULL);
        if(-1==ret)
        {
            write(sockfd,"4 no",MAXSIZE);
            return -1;
        }
        else
        {
            char buf[MAXSIZE]={0};
            sprintf(buf,"4 yes %s",name);
            write(sockfd,buf,sizeof(buf));

            int n,m;
            char **paresult=NULL;
            ret=find_friend_db(db,id,&n,&m,&paresult);
            if(-1==ret)
                return -1;
            else if (1==ret)
                return 1;
            else
            {
                int friendname;
                char ch[2]={0};
                for (int i = 0; i < n; i++)
                {
                    bzero(ch,sizeof(ch));
                    strcpy(ch,paresult[m+2]);
                    if(0==strcmp(ch,"1"))
                    {
                        friendname=atoi(paresult[m]);
                        int friendsockfd;
                        ret=find_sockfd_db(db,friendname,&friendsockfd);
                        if(-1==ret || 1==ret);
                        else
                        {
                            bzero(buf,sizeof(buf));
                            sprintf(buf,"c name %d %s",id,name);
                            write(friendsockfd,buf,sizeof(buf));
                        }
                    }
                }
                return 0;
            }
        }
        
    }
    else if(1==n)
    {
        char aid[50]={0};
        char asize[50]={0};
        sscanf(chmond,"%s%s",aid,asize);
        int id=atoi(aid);
        int size=atoi(asize);
        char iconpath[20]={0};
        sprintf(iconpath,"tem/%d.jpg",id);
        int fd=open(iconpath,O_RDWR|O_CREAT,0666);
        if(-1==fd)
        {
            perror("tem open");
            write(sockfd,"5 no",MAXSIZE);
            return -1;
        }
        
        char buf[MAXSIZE]={0};
        int k=size;
        while (size)
        {
            bzero(buf,sizeof(buf));
            int ret=read(sockfd,buf,sizeof(buf));
            if(-1==ret || 0==ret)
            {
                return 2;
            }
            write(fd,buf,ret);
            size-=ret;
        }
        
        lseek(fd,0,SEEK_SET);
        
        bzero(iconpath,sizeof(iconpath));
        sprintf(iconpath,"image/%d.jpg",id);
        int fd1=open(iconpath,O_RDWR|O_CREAT,0666);
        size=k;
        while (size)
        {
            bzero(buf,sizeof(buf));
            int ret=read(fd,buf,sizeof(buf));
            write(fd1,buf,ret);
            size-=ret;
        }
        close(fd);
        bzero(iconpath,sizeof(iconpath));
        sprintf(iconpath,"tem/%d.jpg",id);
        remove(iconpath);
        bzero(iconpath,sizeof(iconpath));
        sprintf(iconpath,"image/%d.jpg",id);
        int ret=update_user_db(db,id,NULL,iconpath);
        write(sockfd,"5 yes",MAXSIZE);
        // printf("update icon\n");
        int n,m;
        char **paresult=NULL;
        ret=find_friend_db(db,id,&n,&m,&paresult);
        if(-1==ret)
            return -1;
        else if (1==ret)
            return 1;
        else
        {
            int friendname;
            char ch[2]={0};
            for (int i = 0; i < n; i++)
            {
                bzero(ch,sizeof(ch));
                strcpy(ch,paresult[m+2]);
                if(0==strcmp(ch,"1"))
                {
                    friendname=atoi(paresult[m]);
                    int friendsockfd;
                    ret=find_sockfd_db(db,friendname,&friendsockfd);
                    if(-1==ret || 1==ret);
                    else
                    {
                        bzero(buf,sizeof(buf));
                        sprintf(buf,"c icon %s",chmond);
                        write(friendsockfd,buf,sizeof(buf));
                        usleep(1000);
                        lseek(fd1,0,SEEK_SET);
                        size=k;
                        while (size)
                        {
                            bzero(buf,sizeof(buf));
                            ret=read(fd1,buf,sizeof(buf));
                            write(friendsockfd,buf,ret);
                            size-=ret;
                        }
                    }
                }
            }
            
        }
        close(fd1);
        return 0;
    }
}

int send_other(sqlite3 *db,char *chmond,int sockfd)
{
    char flag[2]={0};
    char destid[50]={0};
    char srcdata[MAXSIZE]={0};
    sscanf(chmond,"%s%s%s",flag,destid,srcdata);
    int id=atoi(destid);
    int destsockfd;
    int ret=find_sockfd_db(db,id,&destsockfd);
    if(-1==ret || 1==ret)
        return -1;
    else
    {
        write(destsockfd,chmond,MAXSIZE);
        printf("sendbuf:%s\n",chmond);
        return 0;
    }
    
}

int add_friend(sqlite3 *db,char *chmond,int sockfd)
{
    char flag[2]={0};
    char destid[50]={0};
    char srcid[50]={0};
    sscanf(chmond,"%s%s%s",flag,destid,srcid);
    int did=atoi(destid);
    int sid=atoi(srcid);

    int ret=logon_db(db,did,NULL,NULL);
    if(1!=ret)
    {
        char buf[MAXSIZE]={0};
        sprintf(buf,"b haveno");
        write(sockfd,buf,sizeof(buf));//无该用户
        printf("no haove user\n");
        return -1;
    }

    int destsockfd;
    ret=find_sockfd_db(db,did,&destsockfd);
    if(-1==ret || 1==ret)
    { 
        char buf[MAXSIZE]={0};
        sprintf(buf,"b noline");
        write(sockfd,buf,sizeof(buf));//对方不在线不可加
        return -1;
    }
    else
    {
        // write(destsockfd,chmond,1024);
        // printf("sendbuf:%s\n",chmond);
        char name[50]={0};
        char icon[50]={0};
        ret=add_feiend_info_db(db,sid,name,icon);
        if(0==ret)
        {
            char data[MAXSIZE]={0};
            // sprintf(data,"7 %s %d",name,sid);
            // write(destsockfd,data,sizeof(data));
            // printf("please add friend:%s\n",data);
            // usleep(10000);

            struct stat info;
            if(0 > stat(icon, &info)){
                perror("stat");
                return -1;
            }
            int len = info.st_size;
            sprintf(data,"7 %s %d %d",name,sid,len);
            write(destsockfd,data,sizeof(data));
            printf("please add friend:%s\n",data);
            usleep(10000);
            int size = htonl(len);
            // if(4 != write(destsockfd, &size, 4))
            // {
            //     printf("write head fail.\n");
            //     return -1;;
            // }
            printf("snd file size %dbytes done.\n", len);

            FILE *fp = fopen(icon, "r");
            if(NULL == fp){
                perror("fopen");
                return -1;
            }

            while(1){
                bzero(data,sizeof(data));
                size = fread(data, 1, MAXSIZE, fp);
                if(0 == size){
                    break;
                }
                // printf("%s: read file : size = %d\n", icon, size);

                char *p = data;
                len = size;
                while(len){
                    size = send(destsockfd, p, len, MSG_NOSIGNAL);
                    if(0 >= size){
                        printf("write \"%s\" fail.\n", icon);
                        return -1;
                    }
                    p  += size;
                    len-= size;

                    // printf("%s: size = %d\n", icon, size);
                }
            }
            fclose(fp);
        }

        char buf[20]={0};
        read(destsockfd,buf,sizeof(buf));
        if(0==strncmp(buf+2,"no",2))
        {
            write(sockfd,buf,sizeof(buf));//对方不同意加
            return -1;
        }
        else if (0==strncmp(buf+2,"yes",3))
        {
            // write(sockfd,buf,sizeof(buf));//对方同意加
            // usleep(1000);
            ret=add_friend_db(db,sid,did);

            bzero(name,sizeof(name));
            bzero(icon,sizeof(icon));
            ret=add_feiend_info_db(db,did,name,icon);
            if(0==ret)
            {
                // char data[MAXSIZE]={0};
                // sprintf(data,"b yes %s %d",name,did);
                // write(sockfd,data,MAXSIZE);
                // printf("add success: %s\n",data);
                // usleep(1000);

                struct stat info;
                if(0 > stat(icon, &info)){
                    perror("stat");
                    return -1;
                }
                int len = info.st_size;
                char data[MAXSIZE]={0};
                sprintf(data,"b yes %s %d %d",name,did,len);
                write(sockfd,data,MAXSIZE);
                printf("add success: %s\n",data);
                usleep(1000);
                int size = htonl(len);
                // if(4 != write(sockfd, &size, 4))
                // {
                //     printf("write head fail.\n");
                //     return -1;;
                // }
                printf("snd file size %dbytes done.\n", len);

                FILE *fp = fopen(icon, "r");
                if(NULL == fp){
                    perror("fopen");
                    return -1;
                }

                while(1){
                    bzero(data,sizeof(data));
                    size = fread(data, 1, MAXSIZE, fp);
                    if(0 == size){
                        break;
                    }
                    // printf("%s: read file : size = %d\n", icon, size);

                    char *p = data;
                    len = size;
                    while(len){
                        size = send(sockfd, p, len, MSG_NOSIGNAL);
                        if(0 >= size){
                            printf("write \"%s\" fail.\n", icon);
                            return -1;
                        }
                        p  += size;
                        len-= size;

                        // printf("%s: size = %d\n", icon, size);
                    }
                }
                fclose(fp);
            }

        }
        return 0;
    }
}

int delete_friend(sqlite3 *db,char *chmond,int sockfd)
{
    char did[50]={0};
    char sid[50]={0};
    sscanf(chmond,"%s%s",did,sid);
    int aid=atoi(did);
    int bid=atoi(sid);
    int ret=delete_friend_db(db,aid,bid);
    if(-1==ret)
    {
        write(sockfd,"8 no",MAXSIZE);
        return -1;
    }
    else if (1==ret)
    {
        write(sockfd,"8 haveno",MAXSIZE);
        return -1;
    }
    else
    {
        // printf("delete success\n");
        char buf[MAXSIZE]={0};
        sprintf(buf,"8 yes %d",aid);
        write(sockfd,buf,sizeof(buf));
        int destsockfd;
        ret=find_sockfd_db(db,aid,&destsockfd);
        if(-1==ret || 1==ret)
            return 0;
        else
        {
            bzero(buf,sizeof(buf));
            sprintf(buf,"8 %d",bid);
            write(destsockfd,buf,sizeof(buf));
            printf("delete access %s\n",buf);
            return 0;
        }
        
    }
    
}

int client_exit(sqlite3 *db,char *id)
{
    int myid=atoi(id);
    int n,m;
    char **paresult=NULL;
    int ret=find_friend_db(db,myid,&n,&m,&paresult);
    if(-1==ret)
        return -1;
    else if (1==ret)
        return 1;
    else
    {
        char buf[MAXSIZE]={0};
        int friendname;
        char ch[2]={0};
        for (int i = 0; i < n; i++)
        {
            bzero(ch,sizeof(ch));
            strcpy(ch,paresult[m+2]);
            if(0==strcmp(ch,"1"))
            {
                friendname=atoi(paresult[m]);
                int friendsockfd;
                ret=find_sockfd_db(db,friendname,&friendsockfd);
                if(-1==ret || 1==ret);
                else
                {
                    bzero(buf,sizeof(buf));
                    sprintf(buf,"a %s",id);
                    write(friendsockfd,buf,sizeof(buf));
                }
            }
        }
        return 0;
    }
}

static void send_icon(void *arc)
{
    char *chmond=(char *)arc;
    char aflag[4]={0};
    char aid[50]={0};
    char asocked[4]={0};
    sscanf(chmond,"%s%s%s",aflag,aid,asocked);
    int flag=atoi(aflag);
    int id=atoi(aid);
    int sockfd=atoi(asocked);
    char buf[MAXSIZE]={0};
    if(0==flag)
    {
        char **pazresult=NULL;
        int  n,m;
        int ret=find_friend_db(db1,id,&n,&m,&pazresult);
        if(-1==ret || 1==ret)
        {
            write(sockfd,"0 nofriend(#)",MAXSIZE);
            pthread_exit(NULL);
        }
        else
        {
            int friendname;
            char ch[2]={0};
            usleep(10000);
            for (int i = 0; i < n; i++)
            {
                bzero(ch,sizeof(ch));
                strcpy(ch,pazresult[m+2]);
                if(0==strcmp(ch,"1"))
                {
                    friendname=atoi(pazresult[m]);
                    int friendsockfd;
                    ret=find_sockfd_db(db1,friendname,&friendsockfd);
                    if(-1==ret || 1==ret);
                    else
                    {
                        bzero(buf,sizeof(buf));
                        sprintf(buf,"9 %d",id);
                        write(friendsockfd,buf,sizeof(buf));
                    }
                }

                bzero(buf,sizeof(buf));
                sprintf(buf,"0 %s %s %s",pazresult[m++],pazresult[m++],pazresult[m++]);
                write(sockfd,buf,sizeof(buf));
                printf("friend info%s\n",buf);
                usleep(1000);
                struct stat info;
                if(0 > stat(pazresult[m], &info)){
                    perror("stat");
                    pthread_exit(NULL);
                }
                int len = info.st_size;

                int size = htonl(len);
                if(4 != write(sockfd, &size, 4)){
                    // printf("write head fail.\n");
                    pthread_exit(NULL);
                }
                printf("snd file size %dbytes done.\n", len);

                FILE *fp = fopen(pazresult[m], "r");
                if(NULL == fp){
                    perror("fopen");
                    pthread_exit(NULL);
                }

                while(1){
                    bzero(buf,sizeof(buf));
                    size = fread(buf, 1, MAXSIZE, fp);
                    if(0 == size){
                        break;
                    }
                    // printf("%s: read file : size = %d\n", pazresult[m], size);

                    char *p = buf;
                    len = size;
                    while(len){
                        size = send(sockfd, p, len, MSG_NOSIGNAL);
                        if(0 >= size){
                            // printf("write \"%s\" fail.\n", pazresult[m]);
                            pthread_exit(NULL);
                        }
                        p  += size;
                        len-= size;

                        // printf("%s: size = %d\n",pazresult[m], size);
                    }
                }
                fclose(fp);

                m++;
                usleep(10000);
            }
           pthread_exit(NULL);
        }
    }
}