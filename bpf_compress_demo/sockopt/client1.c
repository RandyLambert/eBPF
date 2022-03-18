#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

#define SERV_PORT 1234
#define SERV_IP "127.0.0.1"
#define MAXLINE 4096
#define MAXSIZE 40

#define IPOPT_TAG 0x21        //IP选项标志字段
#define IPOPT_LEN 8            //IP选项长度字段

int main(int argc,char *argv[])
{
        int sockfd;
        struct sockaddr_in servaddr;

        memset(&servaddr,0,sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(SERV_IP);
        servaddr.sin_port = htons(SERV_PORT);

        //构造自定义的TCP选项
        unsigned char opt[MAXSIZE];
        opt[0] = IPOPT_TAG;
        opt[1] = IPOPT_LEN;
        //写入选项数据
        *(int *)(opt + 4) = htonl(50000);

        if((sockfd = socket(AF_INET,SOCK_STREAM,0)) <= 0){
                perror("socket error : ");
                exit(1);
        }

        if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) < 0){
                perror("connect error ");
                exit(1);
        }

        //设置套接字发送该选项
        if(setsockopt(sockfd,IPPROTO_IP,IP_OPTIONS,(void *)opt,IPOPT_LEN) < 0){
                perror("setsockopt error ");
                exit(1);
        }

        char buff[MAXLINE];

        while(fgets(buff,MAXLINE,stdin) != NULL){
                if(write(sockfd,buff,strlen(buff)) < strlen(buff)){
                        perror("write error ");
                        exit(1);
                }
        }

        close(sockfd);
}