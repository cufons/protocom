//
// Created by cufon on 16.01.24.
//

#include "ProtocolAuthenticationHandler.h"

namespace protocom {
    ProtocolAuthenticationHandler::ProtocolAuthenticationHandler(ProtocolContext &ctx, MessageCoder *coder)
            : ProtocolStateHandler(ctx), coder(coder) {
        request = google::protobuf::Arena::CreateMessage<ClientAuthRequest>(&arena);
        response = google::protobuf::Arena::CreateMessage<ServerAuthResponse>(&arena);
    }

    void ProtocolAuthenticationHandler::handleFrame(PFrame &frame) {
        response->Clear();
        response->set_status(ServerAuthResponse_ResponseStatus_INVALID_REQUEST);
        if(!coder->decode(frame,*request)) {
            std::cout << "Auth failed to decode (who cares)" << std::endl;
        }
        //TODO Implement actual authentication

        respWithStatus(ServerAuthResponse_ResponseStatus_AUTH_ACCEPT);
        ctx.setState(ctx.finalStateFactory.createHandler(ctx,coder));
    }

    void ProtocolAuthenticationHandler::respWithStatus(ServerAuthResponse::ResponseStatus status) {
        response->set_status(status);
        PFrame frame{};
        if (!coder->encode(frame, *response)) throw std::runtime_error("respWithStatus: encode failed");
        ctx.getIO().writeFrame(frame);
    }

} // protocom