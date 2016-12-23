//
//  udphelper.hpp
//  Unity-iPhone
//
//  Created by umt on 16/12/24.
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
    void send();
    void recive();
private:
    int socket_t;
    bool success;
    std::thread recv;
    std::thread send_t;
    uint8_t buffer[1280];
    uint8_t hand[4];
    sockaddr_in addr;
    
};
#endif /* udphelper_hpp */
