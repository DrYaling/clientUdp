//
//  udphelper.cpp
//  Unity-iPhone
//
//  Created by zxb on 16/12/24.
//
//

#include "udphelper.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>
udpHelper::udpHelper()
{
    for (int i = 0; i<20; i++) {
        buffer[i] = (uint8_t*)malloc(2*1024*sizeof(uint8_t));
    }
    memset(bufferSize, 0, 20);
    bufferTail = 0;
}
//udpHelper::~udpHelper()
//{
//    for (int i =0; i<20; i++) {
//        if (buffer[i]!=NULL)
//        {
//            free(buffer[i]);//??
//            buffer[i] = NULL;
//        }
//    }
//}
int udpHelper::Close()
{
    success = false;
//    while (socket_t>0)
    {
        //wait for thread to quit?
    }
    for (int i =0; i<20; i++) {
        memset(buffer[i], 0, 2*1024*sizeof(uint8_t));
    }
    memset(bufferSize, 0, 20);
    bufferTail = 0;
    return  close(socket_t);
}
int udpHelper::Connect(unsigned char* ip,int port)
{
    socket_t = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_t == -1) {
        printf("udp create fail!\n");
        success = false;
        return  2;
    }
    else
    {
        addr.sin_family =AF_INET;
        addr.sin_port = htons(port);
        char str_ip[100] ;
        sprintf(str_ip,"%s", ip);
        success = true;
        addr.sin_addr.s_addr = inet_addr(str_ip);
//        int result = ::connect(socket_t, (struct sockaddr *)&addr, sizeof(addr));
//        bind(socket_t, (struct sockaddr*)&addr, sizeof(addr));
//        printf(" connect to server %s:%d success\n",ip,port);
        receiv = std::thread(&udpHelper::receive,this);
        receiv.detach();
        printf("recv thread started!\n");
    }
    return 0;

}
void udpHelper::receive()
{
//    struct sockaddr_in sins;
//    bzero(&sins,sizeof(sins));
//    sins.sin_family=AF_INET;
//    sins.sin_addr.s_addr=htonl(INADDR_ANY);
//    sins.sin_port=htons(11801);
    printf(" start receive thread\n");
    while (true) {
        if (!success) {
            break;
        }
        //        int size_ = recvfrom(socket_t, buffer, 1280, 0, (struct sockaddr*)&sins,(socklen_t*)&sin_len);
        if (bufferTail>=20) {
            MoveNext();//顶掉第一个包，由上层逻辑重发
        }
        int size_ = recv(socket_t, buffer[bufferTail], 2*1024, 0);
        if (size_>0) {
//            printf("receive message %s from server:%d\n",buffer,size_);
            bufferSize[bufferTail] = size_;
            bufferTail++;
        }
        else if(size_ == -1)
        {
            printf("c socket disconnected!\n");
            break;
            
        }
    }
    socket_t = -1;
    
}
//处理了一个包，将数据前移一位
void udpHelper::MoveNext(){
    for (int i =0; i<19; i++) {
        buffer[i] = buffer[i+1];
        bufferSize[i] = bufferSize[i+1];
    }
    bufferTail -=1;
    ::printf(" tail now is %d",bufferTail);
}
int udpHelper::send(uint8_t* bytes,int length)
{
    if (bytes!=NULL && length>0) {
//        ::printf("send %d data to server \n",length);
        return  sendto(socket_t, bytes,length, 0, (struct sockaddr*)&addr, sizeof(addr));
    }
    return 2;
}
int udpHelper::getBuffer(void* ptr)
{
    if (bufferTail == 0) {
        return 0;
    }
    uint8_t* newbuffer = static_cast<uint8_t*>(ptr) ;
    if (newbuffer == NULL) {
        printf("convert cCharp pointer to uint8_t pointer fail!");
        return -1;
    }
//    ::printf("bufferSize %d,%d\n",bufferSize[0],bufferTail);
    memcpy(newbuffer, buffer[0], bufferSize[0]);
    int totalSize = bufferSize[0];
    MoveNext();
//    int i = 0;
//    ::printf("get ");
//    while (i<totalSize) {
//        ::printf("%0x",newbuffer[i]);
//        i++;
//    }
//    ::printf("get  (%d length) from server \n",totalSize);
    return  totalSize;
}
int udpHelper::getBufferLength()
{
    return  bufferTail;
}
static udpHelper* m_udpHelper[3];
static bool udpHelperUsedFlag[3];
void UdpReset()
{
    for (int i = 0; i<3; i++) {
        m_udpHelper[i] = NULL;
        udpHelperUsedFlag[i] = false;
    }
}
udpHelper* getUdpHelper(int idx)
{
    if (idx>=3) {
        return NULL;
    }

    if (m_udpHelper[idx] == NULL) {
        m_udpHelper[idx] = new udpHelper();
    }
    return m_udpHelper[idx];
}
int udpSend(uint8_t* buff,int length,int idx)
{
    udpHelper* helper = getUdpHelper(idx);
    if (helper!=NULL && length>0 && buff!=NULL) {
        helper->send(buff, length);
        return  0;
    }
    return 2;
}
int udpRecv(void * ptr,int idx)
{
    udpHelper* helper = getUdpHelper(idx);
    if (helper!=NULL) {
        return  helper->getBuffer(ptr);
    }
    return  5;
}
int udpConnect(uint8_t* ip,int port,int idx)
{
    udpHelper* helper = getUdpHelper(idx);
    if (helper!=NULL && ip!=NULL && port>0) {
        return  helper->Connect(ip, port);
    }
    return 1;
}

int udpClose(int idx)
{
    udpHelper* helper = getUdpHelper(idx);
    if (helper!=NULL) {
        //并不销毁，留着下一次使用
        int  ret = helper->Close();
//        if (ret == 0) {
//            delete m_udpHelper;
//            m_udpHelper = nullptr;
//        }
        udpHelperUsedFlag[idx] = false;
        ::printf("socket %d closed\n",idx);
        return  ret;
    }
    ::printf("socket %d close fail!\n",idx);
    return  3;
}
int udpNew()
{
    ::printf("create udpclient\n");
    for (int i = 0; i<3; i++) {
        if (!udpHelperUsedFlag[i]) {
            if (m_udpHelper[i]!=NULL) {
            }
            else
            {
                m_udpHelper[i] = new udpHelper();
            }
            udpHelperUsedFlag[i] = true;
            return i;
        }
    }
//    assert(false);
    return  -1;
}



