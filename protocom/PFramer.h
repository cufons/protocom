//
// Created by cufon on 18.04.24.
//

#ifndef PROTOCOM_PFRAMER_H
#define PROTOCOM_PFRAMER_H

#include "WorkQueue.h"

namespace protocom {

    class PFramer {
        WorkQueue& wq;
        enum RxState {
            RX_HEADER,
            RX_LEN,
            RX_MSG
        } state;
        unsigned msgRemainingBytes;
        unsigned lenRemainingBytes;
        PFrame rxdFrame;

        bool readInProgress;
        bool readEOF;
        bool readAgain;
        void pushFrame();
        void reset();
        int assocFd;
        size_t tryRecv(void* buf, size_t n);
    public:
        explicit PFramer(WorkQueue &wq, int fd);

        bool tryRead();
        void readAll();
        bool isReadEof() const;

        bool isReadAgain() const;

    };

} // protocom

#endif //PROTOCOM_PFRAMER_H
