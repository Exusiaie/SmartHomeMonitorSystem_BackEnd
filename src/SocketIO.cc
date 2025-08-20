#include "SocketIO.hpp"
#include "MyLogger.hpp"
#include <stdio.h>           //perror
#include <sys/socket.h>     //recv
#include <errno.h>           //errno
#include <arpa/inet.h>
#include <iostream>
constexpr int MAX_PACKET_LENGTH = 4096;
using std::cout;
using std::endl;

namespace wd{
//读取包，保存type、length、value原始数据，返回总长度-------------------------------
int SocketIO::readPacket(Packet & packet){
    //TLV:type(4B)|length(4B)|value= 8 + 数据长度

    uint32_t  net_type, net_length;
    //=========8-18新增===========
     // 读取并记录原始网络字节
    // 记录原始网络字节（调试用）
    LogDebug("Raw packet: type=%d, length=%d", net_type, net_length);
    //==============end==========
    readn((char*)&net_type, sizeof(net_type));
    readn((char*)&net_length, sizeof(net_length));
    
    packet.type = ntohl(net_type);        // 网络->主机字节序
    packet.length = ntohl(net_length);     // 网络->主机字节序
    LogDebug("Raw packet: type= %d, length= %d", packet.type, packet.length);

    //==================
      if (packet.length > MAX_PACKET_LENGTH) { // 例如定义MAX_PACKET_LENGTH=4096
        LogError("Invalid packet length: %d", packet.length);
        discardBytes(packet.length);
        return -1;
    }
    //=======================
    if(packet.length > 0){
        char * pbuf = new char[packet.length + 1]();   //new一个数组
        int ret = readn(pbuf,packet.length);           //接收value
        
        packet.msg.assign(pbuf,packet.length);   //length个字节的数据，保存到packet中msg字段中
// 修复问题：丢弃预期长度外的多余数据
        int expectedTotalLength = 8 + 1024; // 头部8B + 固定1024B数据
        int actualReadLength = 8 + ret;
        int remaining = expectedTotalLength - actualReadLength;
        
        if (remaining > 0) {
            discardBytes(remaining);
        }
        // 处理读取不足的情况
        else if (remaining < 0) {
            LogError("Overread detected: %d extra bytes", -remaining);
        }
        
        delete [] pbuf;
        return actualReadLength;
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

// wang::实现丢弃字节的方法
void SocketIO::discardBytes(int n) {
    char dummy[1024];
    while (n > 0) {
        int toRead = n > sizeof(dummy) ? sizeof(dummy) : n;
        int ret = readn(dummy, toRead);
        if (ret <= 0) break; // 错误或EOF
        n -= ret;
    }
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

