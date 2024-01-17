//
// Created by cufon on 12.01.24.
//

#ifndef PROTOCOM_PROTOCOLSTATEHANDLER_H
#define PROTOCOM_PROTOCOLSTATEHANDLER_H

#include "ProtocolContext.h"

namespace protocom {
    class ProtocolContext;
    class ProtocolStateHandler {
    protected:
        ProtocolContext& ctx;
    public:
        explicit ProtocolStateHandler(ProtocolContext& ctx);
        virtual ~ProtocolStateHandler() = default;
        virtual void handleFrame(PFrame& frame) = 0;
    };

} // protocom

#endif //PROTOCOM_PROTOCOLSTATEHANDLER_H
