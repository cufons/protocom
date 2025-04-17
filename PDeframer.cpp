//
// Created by cufon on 29.04.24.
//

#include <cstdio>
#include <system_error>
#include <sys/socket.h>
#include <netinet/in.h>
#include "protocom/PDeframer.h"

namespace protocom {
    protocom::PDeframer::PDeframer() {
        resetAll();
    }

    size_t PDeframer::trySend(void *buf, size_t n) {
        if(n == 0) return 0;
        size_t nsent = send(assocFd,buf,n,0);
        if(!nsent) {
            writeEOF = true;
            return nsent;
        }
        if(nsent == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                writeAgain = true;
                return 0;
            }
            perror("[PDeframer::try_send] send failed");
            throw std::system_error(errno, std::generic_category());
        }
        writeAgain = false;
        return nsent;
    }

    void PDeframer::resetAll() {
        txdFrame.erase();
        assocFd = -1;
        writeInProgress = false;
        resetState();
    }

    void PDeframer::resetState() {
        writeAgain = false;
        writeEOF = false;
        state = TX_HEADER;
        msgRemainingBytes = 0;
        lenRemainingBytes = 0;
    }

    bool PDeframer::isWriteInProgress() const {
        return writeInProgress;
    }

    bool PDeframer::isWriteEof() const {
        return writeEOF;
    }

    bool PDeframer::isWriteAgain() const {
        return writeAgain;
    }

    bool PDeframer::setSentItem(const WorkItem &sentItem) {
        if(writeInProgress) return false;
        txdFrame = sentItem.second;
        assocFd = sentItem.first;
        writeInProgress = true;
        resetState();
        return true;
    }

    bool PDeframer::tryWrite() {
        if(!writeInProgress) return false;
        size_t nwritten;
        switch (state) {
            case TX_HEADER:
                nwritten = trySend(&txdFrame.header, 1);
                if(nwritten == 1) {
                    state = TX_LEN;
                    lenRemainingBytes = 2;
                }
                break;
            case TX_LEN:
                txdFrame.len = htons(txdFrame.len);
                nwritten = trySend(&txdFrame.len + sizeof(PFrame::len) - lenRemainingBytes, lenRemainingBytes);
                lenRemainingBytes -= nwritten;
                if(!lenRemainingBytes) {
                    txdFrame.len = ntohs(txdFrame.len);
                    state = TX_MSG;
                    msgRemainingBytes = txdFrame.len;
                }
                break;
            case TX_MSG:
                nwritten = trySend((void *) (txdFrame.msg + txdFrame.len - msgRemainingBytes), msgRemainingBytes);
                msgRemainingBytes -= nwritten;
                if(!msgRemainingBytes) writeInProgress = false;
        }
        return nwritten > 0;
    }

    bool PDeframer::writeAll() {
        while (tryWrite());
        return !writeInProgress;
    }
} // protocom