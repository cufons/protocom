//
// Created by cufon on 12.01.24.
//

#include "protocom/ProtocolContext.h"
#include "protocom/ProtocolConnectedHandler.h"

namespace protocom {
    ProtocolContext::ProtocolContext(ProtocolUserHandlerFactory *finalStateFactory, Server &serverInstance)
            : io(nullptr), finalStateFactory(finalStateFactory), serverInstance(serverInstance) {
        state = new ProtocolConnectedHandler(*this);
    }

    ProtocolContext::~ProtocolContext() {
        delete io;
        delete state;
    }

    void ProtocolContext::handleFrame(PFrame &frame) {
        if(state == nullptr) return;
        state->handleFrame(frame);
    }

    void ProtocolContext::setState(ProtocolStateHandler* newState) {
        std::cout << "[ProtocolContext::setState] State changed " <<
        (state == nullptr ? "NULL" : typeid(*state).name())
        << "->" <<
        (newState == nullptr ? "NULL" : typeid(*newState).name())  << std::endl;
        delete state;
        state = newState;
    }

    IFrameSink &ProtocolContext::getIO() const {
        if(io == nullptr) throw std::runtime_error("IO is null");
        return *io;
    }

    void ProtocolContext::setIO(IFrameSink *io) {
        ProtocolContext::io = io;
    }

    bool ProtocolContext::isActive() {
        return state;
    }

    Server &ProtocolContext::getServerInstance() const {
        return serverInstance;
    }

    ProtocolUserHandlerFactory *ProtocolContext::getFinalStateFactory() const {
        return finalStateFactory;
    }

} // protocom