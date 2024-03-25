//
// Created by cufon on 17.01.24.
//

#include "protocom/NullUserHandlerFactory.h"

namespace protocom {
    ProtocolStateHandler *NullUserHandlerFactory::createHandler(ProtocolContext &ctx, MessageCoder *coder) const {
        return nullptr;
    }
} // protocom