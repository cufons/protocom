//
// Created by cufon on 17.01.24.
//

#ifndef PROTOCOM_NULLUSERHANDLERFACTORY_H
#define PROTOCOM_NULLUSERHANDLERFACTORY_H

#include "ProtocolUserHandlerFactory.h"

namespace protocom {

    class NullUserHandlerFactory : public ProtocolUserHandlerFactory {
    public:
        ProtocolStateHandler *createHandler(ProtocolContext &ctx, MessageCoder *coder) const override;
    };

} // protocom

#endif //PROTOCOM_NULLUSERHANDLERFACTORY_H
