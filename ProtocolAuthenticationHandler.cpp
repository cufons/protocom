//
// Created by cufon on 16.01.24.
//

#include "protocom/ProtocolAuthenticationHandler.h"
#include "protocom/Server.h"
namespace protocom {
    ProtocolAuthenticationHandler::ProtocolAuthenticationHandler(ProtocolContext &ctx, MessageCoder *coder)
            : ProtocolStateHandler(ctx), coder(coder) {
        request = google::protobuf::Arena::CreateMessage<ClientAuthRequest>(&arena);
        response = google::protobuf::Arena::CreateMessage<ServerAuthResponse>(&arena);
        remainingAttempts = 5;
    }

    void ProtocolAuthenticationHandler::handleFrame(PFrame &frame) {
        response->Clear();
        response->set_status(ServerAuthResponse_ResponseStatus_INVALID_REQUEST);
        if(!coder->decode(frame,*request)) {
            respWithStatus(ServerAuthResponse_ResponseStatus_DECODE_ERROR);
        }
        //TODO Implement actual authentication
        switch (request->request()) {
            case ClientAuthRequest_RequestType_AUTH_SUPPLY:
                doAuth();
                break;
            case ClientAuthRequest_RequestType_AUTH_INFO:
                respWithStatus(ServerAuthResponse_ResponseStatus_INVALID_REQUEST);
                break;
            default:
                respWithStatus(ServerAuthResponse_ResponseStatus_INVALID_REQUEST);
        }

    }

    void ProtocolAuthenticationHandler::doAuth() {
        Authenticator* auth = ctx.getServerInstance().getAuthenticator();
        if(auth == nullptr) {
            proceedAuth();
            return;
        }

        if(auth->authenticate(*request)) {
            proceedAuth();
        } else {
            sleep(1); // Hardcoded sleep to make brute-force difficult
            if (remainingAttempts > 0) {
                remainingAttempts--;
                respWithStatus(ServerAuthResponse_ResponseStatus_AUTH_CONTINUE);
            } else {
                respWithStatus(ServerAuthResponse_ResponseStatus_AUTH_REJECT);
                ctx.setState(nullptr);
            }
        }
    }

    void ProtocolAuthenticationHandler::respWithStatus(ServerAuthResponse::ResponseStatus status) {
        response->set_status(status);
        PFrame frame{};
        if (!coder->encode(frame, *response)) throw std::runtime_error("respWithStatus: encode failed");
        ctx.getIO().writeFrame(frame);
    }

    void ProtocolAuthenticationHandler::proceedAuth() {
        respWithStatus(ServerAuthResponse_ResponseStatus_AUTH_ACCEPT);
        if(ctx.getFinalStateFactory() == nullptr) {
            ctx.setState(nullptr);
        } else {
            ctx.setState(ctx.getFinalStateFactory()->createHandler(ctx, coder));
        }
    }

} // protocom