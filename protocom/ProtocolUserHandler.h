//
// Created by cufon on 17.01.24.
//

#ifndef PROTOCOM_PROTOCOLUSERHANDLER_H
#define PROTOCOM_PROTOCOLUSERHANDLER_H

#include "ProtocolStateHandler.h"
#include "MessageCoder.h"

namespace protocom {
    template<class Req,class Resp>
    class ProtocolUserHandler : public ProtocolStateHandler {
        MessageCoder* coder{};
        void handleFrame(PFrame &frame) override;
        Arena arena;
    protected:
        void sendResp();
        Req* request;
        Resp* response;
    public:
        ProtocolUserHandler(ProtocolContext &ctx, MessageCoder *coder);
        ~ProtocolUserHandler() override;
        virtual void handleMsg(Req& msg) = 0;
        virtual void handleDecodeErr() = 0;
    };

    template<class Req, class Resp>
    ProtocolUserHandler<Req, Resp>::~ProtocolUserHandler() {
        delete coder;
    }

    template<class Req, class Resp>
    ProtocolUserHandler<Req, Resp>::ProtocolUserHandler(ProtocolContext &ctx, MessageCoder *coder) : ProtocolStateHandler(ctx),coder(coder) {
        static_assert(std::is_base_of_v<MessageLite,Req>,"Req must be derived of MessageLite");
        static_assert(std::is_base_of_v<MessageLite,Resp>,"Resp must be derived of MessageLite");
        request = Arena::CreateMessage<Req>(&arena);
        response = Arena::CreateMessage<Resp>(&arena);
    }

    template<class Req, class Resp>
    void ProtocolUserHandler<Req, Resp>::handleFrame(PFrame &frame) {
        if(!coder->decode(frame,*request)) {
            handleDecodeErr();
        } else {
            handleMsg(*request);
        }

    }

    template<class Req, class Resp>
    void ProtocolUserHandler<Req, Resp>::sendResp() {
        PFrame frame{};
        if (!coder->encode(frame, *response)) throw std::runtime_error("respWithStatus: encode failed");
        ctx.getIO().writeFrame(frame);
    }
} // protocom

#endif //PROTOCOM_PROTOCOLUSERHANDLER_H
