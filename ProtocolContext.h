//
// Created by cufon on 12.01.24.
//

#ifndef PROTOCOM_PROTOCOLCONTEXT_H
#define PROTOCOM_PROTOCOLCONTEXT_H

#include "IIOFrame.h"
#include "ProtocolStateHandler.h"
#include "ProtocolUserHandlerFactory.h"

namespace protocom {
    class ProtocolStateHandler;
    class ProtocolUserHandlerFactory;
    class ProtocolContext {
        //friend class ProtocolStateHandler;
        IIOFrame* io{};
    public:
        IIOFrame &getIO() const;
        void setIO(IIOFrame *io);

    private:
        ProtocolStateHandler* state;
    public:
        ProtocolUserHandlerFactory& finalStateFactory;
        explicit ProtocolContext(ProtocolUserHandlerFactory &finalStateFactory);
        void setState(ProtocolStateHandler* newState);
        void handleFrame(PFrame& frame);
        bool isActive();
        ~ProtocolContext();
    };

} // protocom

#endif //PROTOCOM_PROTOCOLCONTEXT_H
