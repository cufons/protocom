//
// Created by cufon on 17.01.24.
//

#include "protocom/TestUserHandler.h"

namespace protocom {
    using string = std::basic_string<char>;
    TestUserHandler::TestUserHandler(ProtocolContext &ctx, MessageCoder *coder) : ProtocolUserHandler(ctx, coder) {}

    void TestUserHandler::handleMsg(UserRequest &msg) {
        response->Clear();
        response->set_msg(string("You said: ") + msg.msg());
        sendResp();
    }

    void TestUserHandler::handleDecodeErr() {
        response->Clear();
        response->set_msg("You stupid!!!!");
        sendResp();
    }

} // protocom