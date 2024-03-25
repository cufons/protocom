//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_SERVERCLIENTHANDLER_H
#define PROTOCOM_SERVERCLIENTHANDLER_H


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdint>
#include <thread>
#include <vector>
#include "IIOFrame.h"
#include "ProtocolContext.h"
#include "MessageCoder.h"
#include "Server.h"

namespace protocom {
    class ServerClientHandler {

        IIOFrame* io;
        int client_fd;
        sockaddr_in cliaddr;
        bool running;
        ProtocolContext ctx;

    public:
        ServerClientHandler(ProtocolUserHandlerFactory &factory, int clientFd, const sockaddr_in &cliaddr,Server& serverInstance);
        ~ServerClientHandler();
        void run();

    };
}

#endif //PROTOCOM_SERVERCLIENTHANDLER_H
