//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_SERVER_H
#define PROTOCOM_SERVER_H

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <thread>
#include <list>
#include <atomic>
#include <mutex>
#include "ProtocolUserHandlerFactory.h"
#include <string>
#include <unordered_map>
#include "ProtocolContext.h"
#include "ServerWorker.h"
#include "PFramer.h"
#include "PDeframer.h"
#include "Authenticator.h"

namespace protocom {
    class ProtocolContext;
    class Server;
    struct ConnectionContext {
        PFramer framer;
        PDeframer deframer;
        bool txReady;
        int fd;
        sockaddr_in client_addr;
        ConnectionContext(int fd, WorkQueue &wqin, WorkQueue &wqout,sockaddr_in& client_addr);
    };
    class Server {
        ServerWorker* worker;
        Authenticator* authenticator;
    public:
        void setAuthenticator(Authenticator *authenticator);

    public:
        Authenticator *getAuthenticator() const;

    private:
        WorkQueue wqIn, wqOut;
        int fd;
        int epollFd;
        int eventFdOut;
        std::string info_string;
        std::mutex fdTableMtx;
        std::unordered_map<int,ConnectionContext> fdInfoTable;
        ProtocolUserHandlerFactory* userHandlerFactory;

        struct sockaddr_in srv_addr{0};
        std::atomic_bool running;

        void acceptClient();
        void closeClient(ConnectionContext& cctx);
        bool createHandler(int clientfd, struct sockaddr_in client_addr);
        void destroyHandler(int clientfd);
        bool epollAddClient(int client_fd);
        bool epollDelClient(int client_fd);
    public:
        bool isRunning() const;

        Server(const char *ip, uint16_t port);

        ~Server();

        bool bindSock();
        void setUserHandlerFactory(ProtocolUserHandlerFactory* factory);
        void run();
        void stop();
        void terminate();
        void closeClient(int fd);
        void setInfoString(const std::string& s);

        int getEventFdOut() const;

        void handleDataIn(ConnectionContext &cctx);

        void handleDataOut();
    };
}

#endif //PROTOCOM_SERVER_H
