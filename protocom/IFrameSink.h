//
// Created by cufon on 17.04.24.
//

#ifndef PROTOCOM_IFRAMESINK_H
#define PROTOCOM_IFRAMESINK_H
#include "IFrameIO.h"
namespace protocom {

    class IFrameSink {
    public:
        virtual bool writeFrame(PFrame& frame) = 0;
        virtual ~IFrameSink() = default;
    };

} // protocom

#endif //PROTOCOM_IFRAMESINK_H
