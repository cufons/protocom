//
// Created by cufon on 11.01.24.
//

#include <cstdio>
#include <stdexcept>
#include <system_error>
#include "ServerClientHandler.h"
#include "IOFrameSocket.h"

namespace protocom {
    ServerClientHandler::ServerClientHandler(ProtocolUserHandlerFactory& factory,int clientFd, const sockaddr_in &cliaddr) : client_fd(clientFd),
                                                                                         cliaddr(cliaddr), ctx(factory) {
        running = true;
        io = new IOFrameSocket(clientFd,1);
        ctx.setIO(io);
    }

    void ServerClientHandler::run() {
        PFrame incomingFrame{0};
        while (running) {
            if (io->isEOF() || !ctx.isActive()) {
                running = false;
                break;
            }
            if (!io->readFrame(incomingFrame)) continue;
            ctx.handleFrame(incomingFrame);
        }
    }

    ServerClientHandler::~ServerClientHandler() {
        delete io;
    }

}