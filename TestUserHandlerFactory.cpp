//
// Created by cufon on 17.01.24.
//

#include "TestUserHandlerFactory.h"
#include "TestUserHandler.h"

namespace protocom {
    ProtocolStateHandler *TestUserHandlerFactory::createHandler(ProtocolContext &ctx, MessageCoder *coder) const {
        return new TestUserHandler(ctx,coder);
    }
} // protocom