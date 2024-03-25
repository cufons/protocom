//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_IIOFRAME_H
#define PROTOCOM_IIOFRAME_H

#include <cstdint>
#include <vector>

namespace protocom {
    struct PFrame {
        uint8_t header;
        uint16_t len;
        const uint8_t *msg;
    };

    class IIOFrame {
    public:
        virtual ~IIOFrame() = default;
        virtual bool writeFrame(PFrame& f) = 0;
        virtual bool readFrame(PFrame& f) = 0;
        virtual bool isEOF() = 0;
    };
}

#endif //PROTOCOM_IIOFRAME_H
