//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_CLIENTHANDLER_H
#define PROTOCOM_CLIENTHANDLER_H


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdint>
#include <thread>
#include <vector>
#include "IIOFrame.h"

namespace protocom {
    class ClientHandler {
        IIOFrame* io;
        int client_fd;
        sockaddr_in cliaddr;
        bool running;
        enum ClientState {
            STATE_INVALID,
            STATE_CONNECTED,
            STATE_ESTABLISHED,
            STATE_AUTHENTICATED
        };
        ClientState state;

        void handleClientConnected();

        void handleClientEstablished();

        void handleClientAuthenticated();

    public:
        ClientHandler(int clientFd, const sockaddr_in &cliaddr);
        ~ClientHandler();
        void run();

    };
}

#endif //PROTOCOM_CLIENTHANDLER_H
