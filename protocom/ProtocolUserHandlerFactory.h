//
// Created by cufon on 17.01.24.
//

#ifndef PROTOCOM_PROTOCOLUSERHANDLERFACTORY_H
#define PROTOCOM_PROTOCOLUSERHANDLERFACTORY_H

#include "ProtocolContext.h"
#include "ProtocolStateHandler.h"
#include "MessageCoder.h"
namespace protocom {
    class ProtocolStateHandler;
    class ProtocolContext;
    class ProtocolUserHandlerFactory {
    public:
        virtual ProtocolStateHandler* createHandler(ProtocolContext& ctx, MessageCoder* coder) const = 0;
    };

} // protocom

#endif //PROTOCOM_PROTOCOLUSERHANDLERFACTORY_H
