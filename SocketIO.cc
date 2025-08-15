#include "SocketIO.hpp"

#include <stdio.h>           //perror
#include <sys/socket.h>     //recv
#include <errno.h>           //errno

namespace wd{

//读取包，保存type、length、value原始数据，返回总长度-------------------------------
int SocketIO::readPacket(Packet & packet){
    //TLV:type(4B)|length(4B)|value= 8 + 数据长度
    int type, length;
    readn((char*)&type,sizeof(type));       //根据长度进行接收type
    readn((char*)&length,sizeof(length));   //根据长度进行接收length

    if(length > 0){
        char * pbuf = new char[length + 1]();   //new一个数组
        int ret = readn(pbuf,length);           //接收value

        packet.type = type;
        packet.length = length;
        packet.msg.assign(pbuf,length);   //length个字节的数据，保存到packet中msg字段中

        delete [] pbuf;                   //new和delete对应，释放数组空间
        return ret + 8;                   //整个TLV的大小
    }

    return 8;
}

//读数据-------------------------
int SocketIO::readn(char * buff,int len){
    int left = len;
    char * pbuf = buff;

    //left= 0退出循环
    while(left > 0){
        int ret = recv(_fd,pbuf,left,0);  //接收数据

        if(ret == -1 && errno == EINTR){ //受信号中断影响
            continue;
        }else if(ret == -1){            //错误情况
            perror("recv");
            return ret;                //还是len - left?
        }else if(ret == 0){             //超时
            return len - left;
        }else{                           //正常
            left -= ret;      //长度变少
            pbuf += ret;      //容量内容变多
        }//if-else if
    }//while

    return len - left;    //?
}

//发送数据------------------------
int SocketIO::sendn(const char * buff,int len){
    int left = len;
    const char * pbuf = buff;

    while(left > 0){
        int ret = send(_fd,pbuf,left,0);

        if(ret == -1 && errno == EINTR){
            continue;
        }else if(ret < 0){       //出错
            perror("send");
            return ret;            //还是len - left ?
        }else{            //正常
            left -= ret;  //长度减少
            pbuf += ret;  //容量空间变大
        }//if-else
    }//while

    return len - left;
}

//读取一行，              最大接收长度
int SocketIO::readline(char * buff,int maxlen){
    int left = maxlen -1;
    char * pbuf = buff;
    int total = 0;

    //有内容的时候---------------------------------------------
    while(left > 0){
        int ret = recvPeek(pbuf,left);  //处理受信号影响的情况

        for(int i = 0;i < ret; ++i){
            if(pbuf[i] == '\n'){      //如果找到'\n'----------
                int sz = i + 1;
                ret = readn(pbuf,sz);  //读出数据

                total += ret;        //长度增加
                pbuf[i] = '\0';      //替换成'\0'

                return total;
            }//if
        }//for

        //这一次没有找到'\n'，从内核接收缓冲区中移走，固定大小的内容？
        ret = readn(pbuf,ret);
        left -= ret;
        pbuf += ret;
        total += ret;
    }//while

    //没有找到'\n'-----------------------------------------------
    buff[maxlen -1] = '\0';
    return maxlen - 1;
}//int

//循环接收数据----------------------------------------
int SocketIO::recvPeek(char * buff,int maxlen) const{
    int ret = 0;

    do{
        ret = recv(_fd,buff,maxlen,MSG_PEEK); //只拷贝
    }while(ret == -1 && errno == EINTR);      //受信号影响的话，继续接收信息

    if(ret < 0){        //出错的话
        perror("recv");
    }

    return ret;
}

}

