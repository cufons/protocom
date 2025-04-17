//
// Created by cufon on 11.01.24.
//

#include <cstring>
#include <iostream>
#include <csignal>
#include "protocom/Server.h"
#include "protocom/NullUserHandlerFactory.h"
#include "sys/epoll.h"
#include "sys/eventfd.h"

namespace protocom {
    Server::Server(const char *ip, uint16_t port) :  running(false),
                                                                     fd(-1),
                                                                     srv_addr{
                                                                             .sin_family = AF_INET,
                                                                             .sin_port = htons(port),
                                                                             .sin_addr = {inet_addr(ip)}

                                                                     },
                                                                     userHandlerFactory(new NullUserHandlerFactory()),
                                                                     info_string("A server."), worker(nullptr), authenticator(nullptr) {
        epollFd = epoll_create(1);
        authenticator = nullptr;

        if (epollFd < 1) {
            throw std::system_error(errno, std::system_category(), "epoll fd failed to open");
        }
        eventFdOut = eventfd(0, EFD_NONBLOCK | EFD_SEMAPHORE);
        if (eventFdOut < 1) {
            throw std::system_error(errno, std::system_category(), "event fd failed to open");
        }
    }

    int Server::getEventFdOut() const {
        return eventFdOut;
    }

    bool Server::bindSock() {
        fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
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

        if (listen(fd, 64) < 0) {
            perror("listen() failed");
            return false;
        }
        epoll_event fdev = {0};
        fdev.data.fd = fd;
        fdev.events = EPOLLIN | EPOLLERR;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &fdev) == -1) {
            perror("epoll_ctl() failed");
            return false;
        };
        fdev.data.fd = eventFdOut;
        fdev.events = EPOLLIN | EPOLLERR;
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, eventFdOut, &fdev) == -1) {
            perror("epoll_ctl() failed");
            return false;
        };
        running = true;
        //signal(SIGINT, [](int sig) { globalStop = true;});
        return true;
    }


    void Server::run() {
        worker = new ServerWorker(wqIn, wqOut, *this, userHandlerFactory,eventFdOut);
        worker->start();
        while (running) {
            std::array<epoll_event, 16> epollEvents;
            int nevents = epoll_wait(epollFd, epollEvents.data(), epollEvents.size(), -1);
            if (nevents == -1 && errno != EINTR) {
                perror("epoll_wait() failed");
                if (errno == EINTR) {
                    continue;
                }
                running = false;
                continue;
            }
            if (!nevents) continue;

            for (int i = 0; i < nevents; i++) {
                epoll_event &ev = epollEvents[i];

                if (ev.data.fd == fd) {
                    // event on server socket
                    if (ev.events & EPOLLERR) {
                        std::cout << "Server socket hangup or error. Stopping..." << std::endl;
                        running = false;
                        continue;
                    }
                    if (ev.events & EPOLLIN) acceptClient();
                    continue;
                }
                if (ev.data.fd == eventFdOut) {
                    // output data available
                    char evCount[8];
                    int nread = read(ev.data.fd,&evCount,sizeof evCount);
                    if(nread == -1) {
                        perror("Read from event fd failed!");
                        continue;
                    }
                    if(nread != sizeof evCount) continue;
                    handleDataOut();
                    continue;
                }
                std::lock_guard<std::mutex> lock(fdTableMtx);
                auto fdInfoIter = fdInfoTable.find(ev.data.fd);
                if (fdInfoIter == fdInfoTable.end()) {
                    std::cout << "Connection context for " << ev.data.fd << " not found" << std::endl;
                    continue;
                }
                ConnectionContext &cctx = fdInfoIter->second;

                // event on client socket

                if (ev.events & EPOLLERR) {
                    std::cout << "Client error. Closing..." << std::endl;
                    closeClient(cctx);
                    continue;
                }

                if(ev.events & EPOLLHUP) {
                    std::cout << "Client hangup. Closing..." << std::endl;
                    closeClient(cctx);
                    continue;
                }

                if (ev.events & EPOLLOUT) {
                    if(cctx.deframer.isWriteInProgress()) {
                        cctx.txReady = cctx.deframer.writeAll();
                    } else {
                        cctx.txReady = true;
                    }
                }

                if (ev.events & EPOLLIN) {
                    handleDataIn(cctx);
                }
            }
        }

    }

    void Server::handleDataOut() {
        WorkItem itmOut;
        if(!wqOut.fetchItemNonBlocking(itmOut)) {
            return;
        }
        std::lock_guard<std::mutex> lock(fdTableMtx);
        auto fdInfoIter = fdInfoTable.find(itmOut.first);
        if (fdInfoIter == fdInfoTable.end()) {
            std::cout << "Connection context for " << itmOut.first << " not found" << std::endl;
            return;
        }
        ConnectionContext &cctx = fdInfoIter->second;
        if(cctx.deframer.setSentItem(itmOut)) {
            cctx.txReady = cctx.deframer.writeAll(); // try writing all the item in one go
        } else {
            std::cout << "Deframer not ready. Dropping item..." << std::endl;
        }
    }

    void Server::handleDataIn(ConnectionContext &cctx) {
        cctx.framer.readAll();
        if(cctx.framer.isReadEof()) {
            std::cout << "Client closed connection." << std::endl;
            closeClient(cctx);

        }
        /*
        WorkItem itm;
        if (wqIn.fetchItemNonBlocking(itm)) {
            std::cout << "Full frame in queue for " << itm.first << " Contains:" << std::endl;
            std::cout << std::hex << itm.second.header << " " << itm.second.len << std::endl;
            for (int i = 0; i < itm.second.len; i++) {
                std::cout << std::hex << itm.second.msg[i] << " ";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Did not receive full frame" << std::endl;
        }
         */
    }

    bool Server::isRunning() const {
        return running;
    }

    void Server::stop() {
        running = false;
        //close(epollFd);
        char notifVal[8] = {0};
        notifVal[0] = 1;
        int res = write(eventFdOut,&notifVal,sizeof notifVal);
        if(worker != nullptr){
            worker->stop();
            delete worker;
            worker = nullptr;
        }
    }

    Server::~Server() {
        //handlerThreads.clear();

        if(worker != nullptr){
            worker->stop();
            delete worker;
            worker = nullptr;
        }
        delete authenticator;
        delete userHandlerFactory;
        if (fd != -1) close(fd);
        close(epollFd);
        close(eventFdOut);
    }

    void Server::destroyHandler(int clientfd) {
        if (worker != nullptr) {
            std::lock_guard<std::mutex> lock(fdTableMtx);
            fdInfoTable.erase(clientfd);
            worker->destroyContext(clientfd);
        }
    }

    bool Server::createHandler(int clientfd, struct sockaddr_in client_addr) {
        if (worker != nullptr) {
            std::lock_guard<std::mutex> lock(fdTableMtx);
            fdInfoTable.emplace(clientfd, ConnectionContext(clientfd, wqIn, wqOut,client_addr));
            worker->createContext(clientfd);
            return true;
        } else {
            return false;
        }
    }

    void Server::setUserHandlerFactory(ProtocolUserHandlerFactory *factory) {
        userHandlerFactory = factory;
    }


    void Server::terminate() {
        running = false;
        worker->stop();
    }

    void Server::setInfoString(const std::string &s) {
        info_string = s;
    }

    void Server::acceptClient() {
        sockaddr_in client_addr = {0};
        socklen_t claddr_len = sizeof(client_addr);
        int client_fd = accept4(fd, (sockaddr *) (&client_addr), &claddr_len, SOCK_NONBLOCK);
        if (client_fd == -1) {
            if (errno == ECONNABORTED) {
                std::cout << "Connection aborted. Skipping..." << std::endl;
                return;
            }
            perror("Accept failed!");
            running = false;
            return;
        }
        if (!epollAddClient(client_fd)) {
            close(client_fd);
        }
        createHandler(client_fd, client_addr);
        std::cout << "Accepted connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port)
                  << " with fd: " << client_fd << std::endl;
    }


    bool Server::epollAddClient(int client_fd) {
        epoll_event epollClientEv{0};
        epollClientEv.data.fd = client_fd;
        epollClientEv.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET;
        int res = epoll_ctl(epollFd, EPOLL_CTL_ADD, client_fd, &epollClientEv);
        if (res == -1) {
            perror("epoll_ctl() add failed");
            return false;
        }
        return true;
    }

    bool Server::epollDelClient(int client_fd) {
        int res = epoll_ctl(epollFd, EPOLL_CTL_DEL, client_fd, nullptr);
        if (res == -1) {
            perror("epoll_ctl() del failed");
            return false;
        }
        return true;
    }

    void Server::closeClient(ConnectionContext& cctx) {
        std::cout << "Closing connection for " << inet_ntoa(cctx.client_addr.sin_addr) << ":" << ntohs(cctx.client_addr.sin_port) << std::endl;
        epollDelClient(cctx.fd);
        close(cctx.fd);
        //destroyHandler(cctx.fd);
        fdInfoTable.erase(cctx.fd);
        if(worker != nullptr) {
            worker->destroyContext(cctx.fd);
        }


    }

    Authenticator *Server::getAuthenticator() const {
        return authenticator;
    }

    void Server::setAuthenticator(Authenticator *authenticator) {
        delete Server::authenticator;
        Server::authenticator = authenticator;
    }

    void Server::closeClient(int fd) {
        auto fdInfoIter = fdInfoTable.find(fd);
        if (fdInfoIter == fdInfoTable.end()) {
            return;
        }
        ConnectionContext& cctx = fdInfoIter->second;
        std::cout << "Closing connection for " << inet_ntoa(cctx.client_addr.sin_addr) << ":" << ntohs(cctx.client_addr.sin_port) << std::endl;
        close(cctx.fd);
        epollDelClient(cctx.fd);
        destroyHandler(cctx.fd);

    }


    ConnectionContext::ConnectionContext(int fd, WorkQueue &wqin, WorkQueue &wqout,sockaddr_in& client_addr) : framer(wqin, fd),fd(fd),
                                                                                                              client_addr(client_addr) {
        txReady = false;
    }
}