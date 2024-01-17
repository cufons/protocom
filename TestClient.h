//
// Created by cufon on 16.01.24.
//

#ifndef PROTOCOM_TESTCLIENT_H
#define PROTOCOM_TESTCLIENT_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "IOFrameSocket.h"
#include "MessageCoder.h"
#include "testuserprotocol.pb.h"

namespace protocom {

    class TestClient {
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
        TestClient(const char* ip, uint16_t port);
        bool connect();
        bool fetchRequest(const UserRequest& req, ServerResponse& resp);
        ~TestClient();
    };

} // protocom

#endif //PROTOCOM_TESTCLIENT_H
