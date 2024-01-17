//
// Created by cufon on 17.01.24.
//

#ifndef PROTOCOM_TESTUSERHANDLERFACTORY_H
#define PROTOCOM_TESTUSERHANDLERFACTORY_H

#include "ProtocolUserHandlerFactory.h"

namespace protocom {

    class TestUserHandlerFactory : public ProtocolUserHandlerFactory {
    public:
    private:
        ProtocolStateHandler *createHandler(ProtocolContext &ctx, MessageCoder *coder) const override;
    };

} // protocom

#endif //PROTOCOM_TESTUSERHANDLERFACTORY_H
