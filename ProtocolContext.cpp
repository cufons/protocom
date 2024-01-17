//
// Created by cufon on 12.01.24.
//

#include "ProtocolContext.h"
#include "ProtocolConnectedHandler.h"

namespace protocom {
    ProtocolContext::ProtocolContext(ProtocolUserHandlerFactory &finalStateFactory)
            : io(nullptr), finalStateFactory(finalStateFactory) {
        state = new ProtocolConnectedHandler(*this);
    }

    ProtocolContext::~ProtocolContext() {
        delete state;
    }

    void ProtocolContext::handleFrame(PFrame &frame) {
        if(state == nullptr) return;
        state->handleFrame(frame);
    }

    void ProtocolContext::setState(ProtocolStateHandler* newState) {
        std::cout << "[ProtocolContext::setState] State changed " << typeid(*state).name() << "->" << typeid(*newState).name() << std::endl;
        delete state;
        state = newState;
    }

    IIOFrame &ProtocolContext::getIO() const {
        if(io == nullptr) throw std::runtime_error("IO is null");
        return *io;
    }

    void ProtocolContext::setIO(IIOFrame *io) {
        ProtocolContext::io = io;
    }

    bool ProtocolContext::isActive() {
        return state;
    }
} // protocom