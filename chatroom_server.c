#include "fun.h" 
typedef struct{
    int  link_fd;
    char username[64];//用户名
    int state;//是否注册用户名
}client_t;
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
    ret = bind(sfd,(struct sockaddr*)&sev_addr,sizeof(sev_addr));
    ERROR_CHECK(ret,-1,"bind");
    ret = listen(sfd,10);
    ERROR_CHECK(ret,-1,"listen");
    int cli_fd;
    //记录已连接用户的fd
    client_t client[64];
    bzero(&client,sizeof(client));
    int num=0;//目前连接客户端数量，用于轮询
    //linkset记录已连接描述符
    fd_set rdset,linkset;
    FD_ZERO(&linkset);
    FD_SET(sfd,&linkset);
    FD_SET(STDIN_FILENO,&linkset);
    char buf[128] = {0};
    char sendbuf[128] = {0};
    while(1){
        FD_ZERO(&rdset);
        memcpy(&rdset,&linkset,sizeof(linkset));
        //printf("test\n");//程序会在select阻塞等待输入
        ret = select(num+4,&rdset,NULL,NULL,NULL);
        ERROR_CHECK(ret,-1,"select");
        if(FD_ISSET(sfd,&rdset)){
            cli_fd = 0;
            struct sockaddr_in cli_addr;
            bzero(&cli_addr,sizeof(cli_addr));
            socklen_t addrlen = sizeof(cli_addr);//获得接入客户端地址
            cli_fd = accept(sfd,(struct sockaddr*)&cli_addr,&addrlen);
            ERROR_CHECK(cli_fd,-1,"accept");
            printf("client ip = %s,client port = %d\n",
                   inet_ntoa(cli_addr.sin_addr),ntohs(cli_addr.sin_port));
            //client[num].link_fd = cli_fd;
            //第一个客户端fd为4，故fd-4=0，类推即为下标
            client[cli_fd-4].link_fd = cli_fd;
            client[cli_fd-4].state = 0;
            bzero(&buf,sizeof(buf));
            //接收用户id
            //ret = recv(cli_fd,buf,sizeof(buf),0);
            //ERROR_CHECK(ret,-1,"recv user id");
            //strcpy(client[cli_fd-4].username,buf);
           // printf("cli_fd=%d,client[%d]=%d\n",cli_fd,num,client[num].link_fd);
            ++num;
            FD_SET(cli_fd,&linkset);//客户fd放入fd集合中
        }
        for(int i=0;i<num;++i){
            if(FD_ISSET(client[i].link_fd,&rdset)){//判断此客户端是否发送消息
                //printf("1,send message:client[%d]=%d\n",i,client[i]);
                bzero(&buf,sizeof(buf));
                ret = recv(client[i].link_fd,buf,sizeof(buf),0);
                ERROR_CHECK(ret,-1,"recv");
                if(ret==0){//recv返回0则表明断开
                    printf("link has closed\n");
                    FD_CLR(client[i].link_fd,&linkset);//删除已断开客户端fd
                    close(client[i].link_fd);
                    bzero(&client[i],sizeof(client_t));
                    --num;
                }else if(client[i].state==0){//保存第一次注册用户id
                    client[i].state=1;
                    strcpy(client[i].username,buf);
                }else{//服务器接收到信息，有客户端发送信息
                    for(int j=0;j<num;++j){//将此客户端信息发送给其他客户端
                        //printf("send to %d\n",j);
                        if(j!=i){
                            bzero(sendbuf,sizeof(sendbuf));
                            strcpy(sendbuf,client[i].username);
                            //ret = send(client[j].link_fd,client[i].username,strlen(client[i].username),0);
                            //ERROR_CHECK(ret,-1,"send to other client id");
                            strcat(sendbuf,":\n");
                            strcat(sendbuf,buf);
                            ret = send(client[j].link_fd,sendbuf,strlen(sendbuf),0);
                            ERROR_CHECK(ret,-1,"send to other client");
                            //printf("2,success send other message.\n");
                        }
                    }
                    printf("%s:\n",client[i].username);
                    printf("%s\n",buf);
                    printf("-----------------------\n");
                }
            }
        }
        if(FD_ISSET(STDIN_FILENO,&rdset)){//服务器发送信息
            bzero(&buf,sizeof(buf));
            ret = read(STDIN_FILENO,buf,sizeof(buf));
            ERROR_CHECK(ret,-1,"read");
            //strlen-1,避免换行符被传送
            for(int j=0;j<num;++j){
                bzero(sendbuf,sizeof(sendbuf));
                strcpy(sendbuf,"sever:\n");
                strcat(sendbuf,buf);
                //ret = send(client[j].link_fd,"sever:",6,0);
                //ERROR_CHECK(ret,-1,"send");
                ret = send(client[j].link_fd,sendbuf,strlen(sendbuf)-1,0);
                ERROR_CHECK(ret,-1,"send");
                if(ret==0){
                    printf("I quit!\n");
                }
            }
            printf("-----------------------\n");
        }
    }
    for(int i=0;i<num;++i){
        close(client[i].link_fd);
    }
    close(sfd);
    return 0;
}

