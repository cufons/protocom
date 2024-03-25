//
// Created by cufon on 12.01.24.
//

#ifndef PROTOCOM_PROTOCOLCONTEXT_H
#define PROTOCOM_PROTOCOLCONTEXT_H

#include "IIOFrame.h"
#include "ProtocolStateHandler.h"
#include "ProtocolUserHandlerFactory.h"
#include "Server.h"

class Server;
namespace protocom {
    class ProtocolStateHandler;
    class ProtocolUserHandlerFactory;
    class ProtocolContext {
        //friend class ProtocolStateHandler;
        IIOFrame* io{};
        Server& serverInstance;
    public:
        Server &getServerInstance() const;
        IIOFrame &getIO() const;
        void setIO(IIOFrame *io);

    private:
        ProtocolStateHandler* state;
    public:
        ProtocolUserHandlerFactory& finalStateFactory;
        explicit ProtocolContext(ProtocolUserHandlerFactory &finalStateFactory, Server &serverInstance);
        void setState(ProtocolStateHandler* newState);
        void handleFrame(PFrame& frame);
        bool isActive();
        ~ProtocolContext();
    };

} // protocom

#endif //PROTOCOM_PROTOCOLCONTEXT_H
