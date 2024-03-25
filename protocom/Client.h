//
// Created by cufon on 16.01.24.
//

#ifndef PROTOCOM_CLIENT_H
#define PROTOCOM_CLIENT_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "IOFrameSocket.h"
#include "MessageCoder.h"

namespace protocom {

    class Client {
        sockaddr_in remAddr;
        int fd;
        bool isConnected;
        IIOFrame* io;
        MessageCoder* coder;
        bool sendMsg(const MessageLite& msg);
        bool fetchMsg(MessageLite& msg);
        bool connect_sock(int socketTimeout = 0);
        static void dump_error(MessageLite& msg);
    public:
        Client(const char* ip, uint16_t port);
        bool connect();
        bool fetchRequest(const MessageLite &req, MessageLite &resp);
        ~Client();
    };

} // protocom

#endif //PROTOCOM_CLIENT_H
