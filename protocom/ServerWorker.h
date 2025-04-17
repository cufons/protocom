//
// Created by cufon on 17.04.24.
//

#ifndef PROTOCOM_SERVERWORKER_H
#define PROTOCOM_SERVERWORKER_H
#include "IFrameSink.h"
#include "WorkQueue.h"
#include "ProtocolContext.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <thread>
namespace protocom {
    class ServerWorker {
        class FrameSink : public IFrameSink {
            WorkQueue& q;
            int notifEventFd;
            int targetFd;
        public:
            FrameSink(WorkQueue& q,int targetFd,int notifEventFd);
            ~FrameSink() override;
            bool writeFrame(PFrame &frame) override;


        };
        WorkQueue &wqIn, &wqOut;
        std::unordered_map<int,ProtocolContext*> handlerTable;
        std::mutex htMutex;
        std::thread* runnerThread;
        ProtocolUserHandlerFactory* userHandlerFactory;
    public:
        void setUserHandlerFactory(ProtocolUserHandlerFactory *userHandlerFactory);

    private:
        Server& serverInstance;
        int notifFd;
        bool shouldStop;
        void run();

    public:
        ServerWorker(WorkQueue &wqIn, WorkQueue &wqOut, Server &serverInstance,
                     ProtocolUserHandlerFactory *userHandlerFactory,int notifFd);
        void start();
        void stop();
        void createContext(int fd);
        void destroyContext(int fd);

    };

} // protocom

#endif //PROTOCOM_SERVERWORKER_H
