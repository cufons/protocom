//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_IFRAMEIO_H
#define PROTOCOM_IFRAMEIO_H

#include <cstdint>
#include <vector>

namespace protocom {
    struct PFrame {
        uint8_t header;
        uint16_t len;
        const uint8_t *msg;
    private:
        bool hasDynamicData;
    public:
        void allocMsg();
        void erase();
        PFrame();
        PFrame(const PFrame& other);
        PFrame(PFrame&& other) noexcept;
        PFrame& operator=(const PFrame& other);
        PFrame& operator=(PFrame&& other) noexcept;
        ~PFrame();
    };

    class IFrameIO {
    public:
        virtual ~IFrameIO() = default;
        virtual bool writeFrame(PFrame& f) = 0;
        virtual bool readFrame(PFrame& f) = 0;
        virtual bool isEOF() = 0;
        virtual bool isWriteExhausted() = 0;
        virtual bool isReadExhausted() = 0;
    };
}

#endif //PROTOCOM_IFRAMEIO_H
