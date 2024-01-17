//
// Created by cufon on 12.01.24.
//

#include "ProtocolConnectedHandler.h"
#include "x25519KexProtocol.h"
#include "EncrMessageCoder.h"
#include "ProtocolAuthenticationHandler.h"

namespace protocom {
    ProtocolConnectedHandler::ProtocolConnectedHandler(ProtocolContext &ctx)
            : ProtocolStateHandler(ctx), isKexComplete(false), response(nullptr), request(nullptr) {
        request = google::protobuf::Arena::CreateMessage<ClientConnectedStateRequest>(&arena);
        response = google::protobuf::Arena::CreateMessage<ServerConnectedStateResponse>(&arena);
        coder = new MessageCoder();
    }

    void ProtocolConnectedHandler::handleFrame(PFrame &frame) {
        response->Clear();
        response->set_status(ServerConnectedStateResponse_ResponseStatus_INVALID_REQUEST);
        if (!coder->decode(frame, *request)) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_DECODE_ERROR);
            return;
        }
        switch (request->type()) {
            case ClientConnectedStateRequest::UNKNOWN:
                break;
            case ClientConnectedStateRequest::REQUEST_KEX:
                handleKex();
                break;
            case ClientConnectedStateRequest::REQUEST_AUTH:
                handleAuthRequest();
                break;
            default:
                break;
        }

    }

    void ProtocolConnectedHandler::handleKex() {
        if (isKexComplete || !request->has_kex()) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_REQUEST_ERROR);
            return;
        }
        x25519KexProtocol kexProto;
        kexProto.init();
        const KexMsg &kexReq = request->kex();
        KexMsg *kexResp = response->mutable_kex();
        if (kexReq.alg() != KexMsg_KexAlg_KEX_ECDH ||
            !kexProto.loadOtherKeyArr(reinterpret_cast<const unsigned char *>(kexReq.pkey().data()),
                                      kexReq.pkey().size())) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_REQUEST_ERROR);
            return;
        }
        kexResp->set_alg(KexMsg_KexAlg_KEX_ECDH);
        const SecByteBlock *pubkey = kexProto.getPubKey();
        if (!pubkey || !kexProto.agree()) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_REQUEST_ERROR);
            return;
        }
        kexResp->set_pkey(string(reinterpret_cast<const char *>(pubkey->data()), pubkey->size()));


        SecByteBlock agreed_key(32);
        if (!kexProto.getKey256(agreed_key)) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_REQUEST_ERROR);
            return;
        }
        respWithStatus(ServerConnectedStateResponse_ResponseStatus_OK);
        isKexComplete = true;
        delete coder;
        coder = new EncrMessageCoder(agreed_key);
    }

    void ProtocolConnectedHandler::handleAuthRequest() {
        if (!isKexComplete) {
            respWithStatus(ServerConnectedStateResponse_ResponseStatus_REQUEST_ERROR);
            return;
        }

        respWithStatus(ServerConnectedStateResponse_ResponseStatus_OK);
        ctx.setState(new ProtocolAuthenticationHandler(ctx, coder));
    }

    ProtocolConnectedHandler::~ProtocolConnectedHandler() {
        //delete coder;
    }

    void ProtocolConnectedHandler::respWithStatus(ServerConnectedStateResponse::ResponseStatus status) {
        response->set_status(status);
        PFrame frame{};
        if (!coder->encode(frame, *response)) throw std::runtime_error("respWithStatus: encode failed");
        ctx.getIO().writeFrame(frame);
    }


} // protocom