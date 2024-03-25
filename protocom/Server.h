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
#include "ProtocolUserHandlerFactory.h"
#include <string>
class ProtocolUserHandlerFactory;
namespace protocom {
    class Server {
        int fd;
        int max_threads;
        std::string info_string;
        std::atomic<int> active_threads;
        std::list<std::thread> handlerThreads;
        ProtocolUserHandlerFactory* userHandlerFactory;

        struct sockaddr_in srv_addr{0};
        bool running;


        bool spawnHandler(int clientfd, struct sockaddr_in client_addr);

        static void handlerThreadWorker(int clientfd, struct sockaddr_in client_addr,std::atomic<int>& active_counter,ProtocolUserHandlerFactory* factory,Server& srvInstance);

    public:
        bool isRunning() const;

        Server(const char *ip, uint16_t port, int max_clients = 1);

        ~Server();

        bool bindSock();
        void setUserHandlerFactory(ProtocolUserHandlerFactory* factory);
        void run();
        void stop();
        void terminate();
        void setInfoString(const std::string& s);
    };
}

#endif //PROTOCOM_SERVER_H
