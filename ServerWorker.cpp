//
// Created by cufon on 17.04.24.
//

#include "protocom/ServerWorker.h"
#include "protocom/Server.h"
#include <sys/prctl.h>
namespace protocom {
    ServerWorker::FrameSink::FrameSink(WorkQueue &q, int targetFd,int notifEventFd) : q(q), targetFd(targetFd),notifEventFd(notifEventFd) {}

    bool ServerWorker::FrameSink::writeFrame(PFrame &frame) {
        q.pushItem(std::make_pair(targetFd,frame));
        char notifVal[8] = {0};
        notifVal[0] = 1;
        int res = write(notifEventFd,&notifVal,sizeof notifVal);
        if(res == -1) {
            perror("Notification send failed!!!");
            return false;
        }
        return true;
    }

    ServerWorker::FrameSink::~FrameSink() {

    }

    void ServerWorker::run() {
        prctl(PR_SET_NAME,"protcom worker",NULL,NULL,NULL);
        while (!shouldStop) {
            WorkItem incomingMsg;
            try {
                wqIn.fetchItemBlocking(incomingMsg);
            } catch (WorkQueue::CancelledException& e) {
                continue;
            }
            //std::cout << "Worker received message for fd: " << incomingMsg.first << std::endl;
            auto handlerIter = handlerTable.find(incomingMsg.first);
            if(handlerIter == handlerTable.end()) {
                std::cout << "Error! Couldn't find handler for socket " << incomingMsg.first << ", skipping..." << std::endl;
                continue;
            }

            if(!handlerIter->second->isActive()) {
                handlerTable.erase(handlerIter);
                continue;
            }
            handlerIter->second->handleFrame(incomingMsg.second);
            if(!handlerIter->second->isActive()) {
                serverInstance.closeClient(incomingMsg.first);
            }
        }
        std::cout << "Worker thread exiting..." << std::endl;
    }

    void ServerWorker::stop() {
        shouldStop = true;
        wqIn.cancelFetch();
        if(runnerThread) {
            runnerThread->join();
            delete runnerThread;
            runnerThread = nullptr;
        }
    }

    ServerWorker::ServerWorker(WorkQueue &wqIn, WorkQueue &wqOut, Server &serverInstance,
                               ProtocolUserHandlerFactory *userHandlerFactory,int notifFd)
            : wqIn(wqIn), wqOut(wqOut), serverInstance(serverInstance), userHandlerFactory(userHandlerFactory),runnerThread(
            nullptr),notifFd(notifFd) {
        shouldStop = false;
    }

    void ServerWorker::createContext(int fd) {
        std::lock_guard<std::mutex> lock(htMutex);
        auto* ctx = new ProtocolContext(userHandlerFactory,serverInstance);
        ctx->setIO(new FrameSink(wqOut,fd,notifFd));
        handlerTable.emplace(fd,ctx);

    }

    void ServerWorker::destroyContext(int fd) {
        std::lock_guard<std::mutex> lock(htMutex);
        handlerTable.erase(fd);
    }

    void ServerWorker::setUserHandlerFactory(ProtocolUserHandlerFactory *userHandlerFactory) {
        this->userHandlerFactory = userHandlerFactory;
    }

    void ServerWorker::start() {
        runnerThread = new std::thread(&ServerWorker::run, this);
    }

} // protocom