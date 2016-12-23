//
//  udphelper.cpp
//  Unity-iPhone
//
//  Created by umt on 16/12/24.
//
//

#include "udphelper.hpp"
#include <unistd.h>
#include <arpa/inet.h>
udpHelper::udpHelper()
{
    memset(buffer, 0, 1280);
    hand[0] = 233;
    hand[1] = 0;
    hand[2] = 0;
    hand[3] = 0;
    addr.sin_family =AF_INET;
    addr.sin_port = htons(11801);
    addr.sin_addr.s_addr = inet_addr("10.96.178.42");
    socket_t = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_t == -1) {
        printf("udp create fail!\n");
        success = false;
    }
    else
    {
        success = true;
        //        bind(socket_t, (struct sockaddr*)&addr, sizeof(addr));
        sendto(socket_t, hand, 4, 0, (struct sockaddr*)&addr, sizeof(addr));
        recv = std::thread(&udpHelper::recive,this);
        recv.detach();
        send_t = std::thread(&udpHelper::send,this);
        send_t.detach();
        printf("recv thread started!\n");
    }
}
void udpHelper::recive()
{
    static int _time_t = 0;
    static int tick = 0;
    struct sockaddr_in sins;
    printf("Waiting for data form sender \n");
    
    bzero(&sins,sizeof(sins));
    sins.sin_family=AF_INET;
    sins.sin_addr.s_addr=htonl(INADDR_ANY);
    sins.sin_port=htons(11801);
    int sin_len=sizeof(sins);
//    bind(socket_t,(struct sockaddr *)&sins,sizeof(sins));
    printf(" start receive thread");
    while (true) {
//        time_t tim = 0;
        int size_ = recvfrom(socket_t, buffer, 1280, 0, (struct sockaddr*)&sins,(socklen_t*)&sin_len);
        if (size_>0) {
            printf("receive message from server:%d\n",size_);
            memset(buffer, 0, 1280);
        }
        else if(size_ == -1)
        {
            printf("socket disconnected!\n");
            
        }
        _time_t+=1;
//        tick+=10;
//        if (_time_t>6000) {
//            sendto(socket_t, hand, 4, 0, (struct sockaddr*)&addr, sizeof(addr));
//            printf("send hand shake to server at %d \n",tick/1000);
//            _time_t = 0;
//        }
    }
    
}
void udpHelper::send()
{
    while (true) {
        sleep(6);
        sendto(socket_t, hand, 4, 0, (struct sockaddr*)&addr, sizeof(addr));
        printf("send hand shake to server \n");
    }
}
