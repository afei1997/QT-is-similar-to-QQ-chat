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

#define MAXSIZE 1024

int sign_in(sqlite3 *db,char *chmond,int sockfd)
{
    char id[20]={0};
    char pass[50]={0};
    sscanf(chmond,"%s%s",id,pass);
    int userid=atoi(id);
    char name[50]={0};
    char icon[50]={0};
    int ret=sign_in_db(db,userid,pass,name,icon,sockfd);
    if(-1==ret)
    {
        write(sockfd,"no",5);
        return -1;
    }
    else if (1==ret)
    {
        write(sockfd,"online",10);
        return 1;
    }
    else
    {
        write(sockfd,"yes",5);
        usleep(10000);
        char buf[MAXSIZE]={0};
        write(sockfd,name,sizeof(name));
        usleep(10000);
        struct stat info;
		if(0 > stat(icon, &info)){
			perror("stat");
			return -1;
		}

		int len = info.st_size;
		int size = htonl(len);
		if(4 != write(sockfd, &size, 4)){
			printf("write head fail.\n");
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
			printf("%s: read file : size = %d\n", icon, size);

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

				printf("%s: size = %d\n", icon, size);
			}
		}
		fclose(fp);

        usleep(10000);
        char **pazresult=NULL;
        int  n,m;
        ret=find_friend_db(db,userid,&n,&m,&pazresult);
        if(-1==ret || 1==ret)
        {
            write(sockfd,"nofriend",10);
            return 0;
        }
        else
        {
            bzero(buf,sizeof(buf));
            sprintf(buf,"%d",n);
            printf("useid=%d n=%d buf=%s\n",userid,n,buf);
            write(sockfd,buf,sizeof(buf));
            int friendname;
            char ch[2]={0};
            for (int i = 0; i < n; i++)
            {
                bzero(ch,sizeof(ch));
                strcpy(ch,pazresult[m+2]);
                if(0==strcmp(ch,"1"))
                {
                    friendname=atoi(pazresult[m]);
                    int friendsockfd;
                    ret=find_sockfd_db(db,friendname,&friendsockfd);
                    if(-1==ret || 1==ret);
                    else
                    {
                        bzero(buf,sizeof(buf));
                        sprintf(buf,"9 %d",userid);
                        write(friendsockfd,buf,sizeof(buf));
                    }
                }

                bzero(buf,sizeof(buf));
                sprintf(buf,"%s %s %s",pazresult[m++],pazresult[m++],pazresult[m++]);
                write(sockfd,buf,sizeof(buf));
                usleep(1000);

                int len = info.st_size;
                int size = htonl(len);
                if(4 != write(sockfd, &size, 4)){
                    printf("write head fail.\n");
                    return -1;;
                }
                printf("snd file size %dbytes done.\n", len);

                FILE *fp = fopen(pazresult[m], "r");
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
                    printf("%s: read file : size = %d\n", pazresult[m], size);

                    char *p = buf;
                    len = size;
                    while(len){
                        size = send(sockfd, p, len, MSG_NOSIGNAL);
                        if(0 >= size){
                            // printf("write \"%s\" fail.\n", pazresult[m]);
                            return -1;
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
            return 0;
        }
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
        write(sockfd,"no",2);
        return -1;
    }
    else if (1==ret)
    {
        write(sockfd,"existing",10);
        return 1;
    }
    else
    {
        write(sockfd,"yes",5);
        return 0;
    }
}

int update_user(sqlite3 *db,char *chmond,char *id,int n,int sockfd)
{
    if(0==n)
    {
        
    }
}

int send_other(sqlite3 *db,char *chmond,int sockfd)
{
    char flag[2]={0};
    char destid[50]={0};
    char srcdata[1024]={0};
    sscanf(chmond,"%s%s%s",flag,destid,srcdata);
    int id=atoi(destid);
    int destsockfd;
    int ret=find_sockfd_db(db,id,&destsockfd);
    if(-1==ret || 1==ret)
        return -1;
    else
    {
        write(destsockfd,chmond,1024);
        printf("sendbuf:%s\n",chmond);
        return 0;
    }
    
}

int add_friend(sqlite3 *db,char *chmond,int sockfd);

int delete_friend(sqlite3 *db,char *chmond,int sockfd);

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