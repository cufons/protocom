//
// Created by cufon on 16.01.24.
//

#ifndef PROTOCOM_PROTOCOLAUTHENTICATIONHANDLER_H
#define PROTOCOM_PROTOCOLAUTHENTICATIONHANDLER_H

#include "ProtocolContext.h"
#include "MessageCoder.h"
#include "pcomserver.pb.h"

namespace protocom {

    class ProtocolAuthenticationHandler : public ProtocolStateHandler {
        MessageCoder* coder;
        ServerAuthResponse* response;
        ClientAuthRequest* request;
        google::protobuf::Arena arena;
        int remainingAttempts;
        void respWithStatus(ServerAuthResponse::ResponseStatus status);
        void proceedAuth();
    public:
        ProtocolAuthenticationHandler(ProtocolContext &ctx, MessageCoder *coder);
        void handleFrame(PFrame &frame) override;

        void doAuth();
    };

} // protocom

#endif //PROTOCOM_PROTOCOLAUTHENTICATIONHANDLER_H
