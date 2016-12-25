//
//  udphelper.hpp
//  Unity-iPhone
//
//  Created by zxb on 16/12/24.
//
//

#ifndef udphelper_hpp
#define udphelper_hpp

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <thread>
#include <netinet/in.h>
class udpHelper{
public:
    udpHelper();
    ~udpHelper();
    int send(uint8_t* bytes,int length);
    void receive();
    int Close();
    int Connect(unsigned char* ip,int port);
    inline int getBufferLength();
    inline int getBuffer(void* ptr);
    void MoveNext();
private:
    int socket_t;
    bool success;
    std::thread receiv;
    std::thread send_t;
    uint8_t* buffer[20];
    int bufferSize[20];
    sockaddr_in addr;
    uint8_t bufferHead;
    uint8_t bufferTail;
    
};
extern "C"{
//    typedef void(*ReceiveCallBack)(uint8_t* p);
//    typedef void(*ExceptionCallback)(int p);
    int udpSend(uint8_t* buff,int length,int idx);
    int udpRecv(void* ptr,int idx);
    int udpClose(int idx);
    int udpConnect(uint8_t* ip,int port,int idx);
    int udpNew();
    void UdpReset();
    
}
#endif /* udphelper_hpp */
