//
// Created by cufon on 11.01.24.
//

#include <cstdio>
#include <stdexcept>
#include <system_error>
#include "ClientHandler.h"
#include "IOFrameSocket.h"

namespace protocom {
    ClientHandler::ClientHandler(int clientFd, const sockaddr_in &cliaddr) : client_fd(clientFd),
                                                                             cliaddr(cliaddr) {
        running = true;
        io = new IOFrameSocket(clientFd);
    }

    void ClientHandler::run() {
        state = STATE_CONNECTED;
        PFrame incomingFrame{0};
        while (running) {
            if (!io->isOpen()) {
                running = false;
                break;
            }
            if (!io->readFrame(incomingFrame)) continue;
            io->writeFrame(incomingFrame);
            switch (state) {
                case STATE_INVALID:
                    running = false;
                    break;
                case STATE_CONNECTED:
                    handleClientConnected();
                    break;
                case STATE_AUTHENTICATED:
                    handleClientAuthenticated();
                    break;
                case STATE_ESTABLISHED:
                    handleClientEstablished();
                    break;
            }
        }
    }

    void ClientHandler::handleClientConnected() {
    }

    void ClientHandler::handleClientEstablished() {

    }

    void ClientHandler::handleClientAuthenticated() {

    }

    ClientHandler::~ClientHandler() {
        delete io;
    }
}