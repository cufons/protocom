//
// Created by cufon on 17.01.24.
//

#ifndef PROTOCOM_TESTUSERHANDLER_H
#define PROTOCOM_TESTUSERHANDLER_H
#include "testuserprotocol.pb.h"
#include "ProtocolUserHandler.h"

namespace protocom {

    class TestUserHandler : public ProtocolUserHandler<UserRequest,ServerResponse> {
    public:
        TestUserHandler(ProtocolContext &ctx, MessageCoder *coder);

    private:
        void handleMsg(UserRequest &msg) override;
        void handleDecodeErr() override;
    };

} // protocom

#endif //PROTOCOM_TESTUSERHANDLER_H
