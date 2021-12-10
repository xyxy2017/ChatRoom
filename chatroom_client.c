#include "fun.h"
int main(int argc,char *argv[])
{
    ARGS_CHECK(argc,3);
    int sfd = socket(AF_INET,SOCK_STREAM,0);
    ERROR_CHECK(sfd,-1,"socket");
    struct sockaddr_in sev_addr;
    bzero(&sev_addr,sizeof(sev_addr));
    sev_addr.sin_family = AF_INET;
    sev_addr.sin_port = htons(atoi(argv[2]));
    sev_addr.sin_addr.s_addr = inet_addr(argv[1]);
    int ret;
    ret = connect(sfd,(struct sockaddr*)&sev_addr,sizeof(sev_addr));
    ERROR_CHECK(sfd,-1,"accept");
    fd_set rdset;
    char buf[128] = {0};
    //注册用户名
    printf("Please input your username:\n");
    read(STDIN_FILENO, buf, sizeof(buf));
    ret = send(sfd, buf, strlen(buf) - 1, 0);
    ERROR_CHECK(ret, -1, "send");
    //将buf的最后一个换行符去掉
    buf[strlen(buf) - 1]=0;
    printf("register success,your id=%s\n",buf);
    printf("------------------enter chatroom-----------------\n");
    while(1){
        FD_ZERO(&rdset);
        FD_SET(sfd,&rdset);
        FD_SET(STDIN_FILENO,&rdset);
        //printf("test\n");//证明在输入和接受会在select前睡眠
        ret = select(sfd+1,&rdset,NULL,NULL,NULL);
        ERROR_CHECK(ret,-1,"select");
        if(FD_ISSET(sfd,&rdset)){
           // printf("get message.\n");
            bzero(&buf,sizeof(buf));
            ret = recv(sfd,buf,sizeof(buf),0);
            ERROR_CHECK(ret,-1,"recv");
            if(ret==0){
                printf("link has closed!\n");
                break;
            }
            printf("%s\n",buf);
            printf("--------------------\n");
        }
        if(FD_ISSET(STDIN_FILENO,&rdset)){
            bzero(&buf,sizeof(buf));
            ret = read(STDIN_FILENO,buf,sizeof(buf));
            ERROR_CHECK(ret,-1,"read");
            if(ret==0){
                printf("I close link!\n");
                break;
            }
            ret = send(sfd,buf,strlen(buf)-1,0);
            ERROR_CHECK(ret,-1,"send");
            printf("--------------------\n");
        }
    }
    //close(sfd);
    return 0;
}

