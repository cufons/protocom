//
// Created by cufon on 29.04.24.
//

#ifndef PROTOCOM_PDEFRAMER_H
#define PROTOCOM_PDEFRAMER_H

#include "WorkQueue.h"

namespace protocom {

    class PDeframer {
        enum RxState {
            TX_HEADER,
            TX_LEN,
            TX_MSG
        } state;

        unsigned msgRemainingBytes;
        unsigned lenRemainingBytes;
        PFrame txdFrame;
        bool writeInProgress;
        bool writeEOF;
        bool writeAgain;

        void resetAll();
        int assocFd;
        size_t trySend(void* buf, size_t n);
    public:
        PDeframer();


        bool tryWrite();
        bool writeAll();
        bool setSentItem(const WorkItem &sentItem);
        bool isWriteInProgress() const;

        bool isWriteEof() const;

        bool isWriteAgain() const;

        void resetState();
    };

} // protocom

#endif //PROTOCOM_PDEFRAMER_H
