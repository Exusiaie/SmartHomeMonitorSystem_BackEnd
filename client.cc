#include <cstdlib>
#include <sys/select.h>
#include <unistd.h>      //close
#include <netinet/in.h>  //htons
#include <sys/socket.h>
#include <cstring>       //memset
#include <arpa/inet.h>   //inet_addr 

#include <iostream>
using std::cout;
using std::endl;

int main()
{
    //生成套接字-----------------------------------
    int clientfd = socket(AF_INET,SOCK_STREAM,0);
    if(clientfd < 0){
        perror("socket");
        return EXIT_FAILURE;  //失败，退出
    }

    //结构体，端口、地址------------------------
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(8000);     //端口，主机字节序->网络字节序 
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //地址，点分十进制->网络字节序

    //建立连接-----------------------------------
    int ret = connect(clientfd,(const struct sockaddr*)&serveraddr,sizeof(serveraddr));
    if(ret < 0){
        perror("connect");
        close(clientfd);
        return EXIT_FAILURE;
    }

    printf("connect success\n");

    fd_set readset;
    FD_ZERO(&readset);
    char buff[1024] = {0};
    while(1){
        FD_ZERO(&readset);
        FD_SET(clientfd,&readset);      //?
        FD_SET(STDIN_FILENO,&readset);  //?

        int nready = select(clientfd + 1,&readset,NULL,NULL,NULL);
        printf("select nready %d\n",nready);

        //如果是标准输入-----------------------
        if(FD_ISSET(STDIN_FILENO,&readset)){
            memset(buff,0,sizeof(buff));
            ret = read(STDIN_FILENO,buff,sizeof(buff));
            printf("read %d bytes.\n",ret);

            ret = send(clientfd,buff,strlen(buff),0);
            printf("send %d bytes.\n",ret);
        }//if

        //如果是用户fd-------------------------
        if(FD_ISSET(clientfd,&readset)){
            memset(buff,0,sizeof(buff));
            ret = recv(clientfd,buff,sizeof(buff),0);
            if(ret == 0){
                break;
            }

            printf("recv from server:%s,%d bytes.\n",buff,ret);
        }//if
    }//while

    close(clientfd);
    return 0;
}

