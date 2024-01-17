//
// Created by cufon on 12.01.24.
//

#ifndef PROTOCOM_PROTOCOLCONNECTEDHANDLER_H
#define PROTOCOM_PROTOCOLCONNECTEDHANDLER_H

#include "ProtocolStateHandler.h"
#include "pcomserver.pb.h"
#include "MessageCoder.h"
#include <cryptopp/xed25519.h>

namespace protocom {

    class ProtocolConnectedHandler : public ProtocolStateHandler {
        google::protobuf::Arena arena;
        MessageCoder* coder;
        ClientConnectedStateRequest* request;
        ServerConnectedStateResponse* response;
        bool isKexComplete;
        void handleKex();
        void handleAuthRequest();
        void respWithStatus(ServerConnectedStateResponse::ResponseStatus status);
    public:
        explicit ProtocolConnectedHandler(ProtocolContext &ctx);
        void handleFrame(PFrame& frame)override;
        ~ProtocolConnectedHandler() override;
    };

} // protocom

#endif //PROTOCOM_PROTOCOLCONNECTEDHANDLER_H
