#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <syslog.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <strings.h>
#include <sys/epoll.h>
#define ARGS_CHECK(argc,num){if(argc!=num){\
    fprintf(stderr,"Args Error!\n");\
    return -1;\
}}
#define ERROR_CHECK(ret,num,msg){if(ret==num){\
        perror(msg);\
        return -1;\
}}
#define THREAD_ERROR_CHECK(ret,funcname){if(ret!=0){\
    printf("%s failed %s\n",funcname,strerror(ret));\
    return -1;\
}}
