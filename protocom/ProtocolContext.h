//
// Created by cufon on 12.01.24.
//

#ifndef PROTOCOM_PROTOCOLCONTEXT_H
#define PROTOCOM_PROTOCOLCONTEXT_H

#include "IFrameSink.h"
#include "ProtocolStateHandler.h"
#include "ProtocolUserHandlerFactory.h"


namespace protocom {
    class Server;
    class ProtocolStateHandler;
    class ProtocolUserHandlerFactory;
    class ProtocolContext {
        //friend class ProtocolStateHandler;
        IFrameSink* io{};
        Server& serverInstance;
    public:
        Server &getServerInstance() const;
        IFrameSink &getIO() const;
        void setIO(IFrameSink *io);

    private:
        ProtocolStateHandler* state;
    public:
        ProtocolUserHandlerFactory* finalStateFactory;

        ProtocolUserHandlerFactory *getFinalStateFactory() const;

        explicit ProtocolContext(ProtocolUserHandlerFactory *finalStateFactory, Server &serverInstance);
        void setState(ProtocolStateHandler* newState);
        void handleFrame(PFrame& frame);
        bool isActive();
        ~ProtocolContext();
    };

} // protocom

#endif //PROTOCOM_PROTOCOLCONTEXT_H
