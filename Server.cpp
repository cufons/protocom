//
// Created by cufon on 11.01.24.
//

#include <cstring>
#include <iostream>
#include "Server.h"
#include "ServerClientHandler.h"
#include "NullUserHandlerFactory.h"

namespace protocom {
    Server::Server(const char *ip, uint16_t port, int max_clients) : max_threads(max_clients),active_threads(0), running(false), fd(-1),
                                                               srv_addr{
                                                                       .sin_family = AF_INET,
                                                                       .sin_port = htons(port),
                                                                       .sin_addr = {inet_addr(ip)}

                                                               },userHandlerFactory(new NullUserHandlerFactory()) {}

    bool Server::bindSock() {
        fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
            perror("[Server] socket() failed");
            return false;
        }
        if (bind(fd, (sockaddr *) &srv_addr, sizeof(srv_addr)) < 0) {
            perror("bind() failed");
            return false;
        } else {
            printf("Server bound on %s:%d\n", inet_ntoa(srv_addr.sin_addr), ntohs(srv_addr.sin_port));
        }

        if (listen(fd, max_threads) < 0) {
            perror("listen() failed");
            return false;
        }
        running = true;
        return true;
    }

    void Server::run() {
        while (running) {
            sockaddr_in client_addr = {0};
            socklen_t claddr_len = sizeof(client_addr);
            int client_fd = accept(fd, (sockaddr *) (&client_addr), &claddr_len);
            if (fd == -1) {
                if (errno == ECONNABORTED) {
                    puts("Connection aborted. Skipping...");
                    continue;
                }
                perror("Accept failed!");
                running = false;
                continue;
            }
           printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            if (!spawnHandler(client_fd, client_addr)) {
                std::cout << "Max connections reached. Closing!" << std::endl;
                close(client_fd);
            }

        }
    }

    bool Server::isRunning() const {
        return running;
    }

    void Server::stop() {
        running = false;
        for (auto &t: handlerThreads) t.join();
    }

    Server::~Server() {
        delete userHandlerFactory;
        if (fd != -1) close(fd);
    }

    bool Server::spawnHandler(int clientfd, struct sockaddr_in client_addr) {
        if (active_threads >= max_threads) return false;
        handlerThreads.emplace_back(Server::handlerThreadWorker, clientfd, client_addr,std::ref(active_threads), userHandlerFactory);
        return true;
    }

    void Server::handlerThreadWorker(int clientfd, struct sockaddr_in client_addr,std::atomic<int>& active_counter,ProtocolUserHandlerFactory* factory) {
        ServerClientHandler connHandler(*factory,clientfd, client_addr);
        active_counter++;
        std::cout << "Worker stared" << std::endl;
        try {
            connHandler.run();
        }
        catch (std::exception& e) {
            std::cout << "An exception occured during client handling:" << std::endl;
            std::cout << e.what() << std::endl;
        }
        std::cout << "Worker exiting" << std::endl;
        active_counter--;
        close(clientfd);
    }

    void Server::setUserHandlerFactory(ProtocolUserHandlerFactory *factory) {
        userHandlerFactory = factory;
    }
}