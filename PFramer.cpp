//
// Created by cufon on 18.04.24.
//

#include <unistd.h>
#include <sys/socket.h>
#include <cstdio>
#include <system_error>
#include <netinet/in.h>
#include <iostream>
#include "protocom/PFramer.h"

protocom::PFramer::PFramer(protocom::WorkQueue &wq, int fd) : wq(wq), assocFd(fd) {
    reset();
}

bool protocom::PFramer::tryRead() {
    size_t nread;
    switch (state) {
        case RX_HEADER:
            nread = tryRecv(&rxdFrame.header, 1);
            if(nread == 1 ) {
                if((rxdFrame.header & 0xF0) != 0xF0) { // Also check for header validity
                    return false;
                }
                state = RX_LEN;
                lenRemainingBytes = 2;
            }
            break;
        case RX_LEN:
            nread = tryRecv(&rxdFrame.len + sizeof(PFrame::len) - lenRemainingBytes, lenRemainingBytes);
            rxdFrame.len = ntohs(rxdFrame.len);
            lenRemainingBytes -= nread;
            if(!lenRemainingBytes) {
                state = RX_MSG;
                msgRemainingBytes = rxdFrame.len;
                rxdFrame.allocMsg();
            }
            break;
        case RX_MSG:
            nread = tryRecv((void *) (rxdFrame.msg + rxdFrame.len - msgRemainingBytes), msgRemainingBytes);
            msgRemainingBytes -= nread;
            if(!msgRemainingBytes) pushFrame();
            break;
    }
    return nread > 0;
}

size_t protocom::PFramer::tryRecv(void *buf, size_t n) {
    if(n == 0) return 0;
    ssize_t nrecv = recv(assocFd, buf, n, 0);
    if(!nrecv) {
        readEOF = true;
        return nrecv;
    }
    if (nrecv == -1) {
        if(errno == EAGAIN || errno == EWOULDBLOCK) {
            readAgain = true;
            return 0;
        }
        perror("[tryRecv] recv failed");
        throw std::system_error(errno, std::generic_category());
    }
    readEOF = false;
    return nrecv;
}

void protocom::PFramer::pushFrame() {
    wq.pushItem(std::make_pair(assocFd,std::move(rxdFrame)));
    reset();
}

void protocom::PFramer::reset() {
    rxdFrame.erase();
    readInProgress = false;
    readEOF = false;
    readAgain = false;
    state = RX_HEADER;
    msgRemainingBytes = 0;
    lenRemainingBytes = 0;
}

bool protocom::PFramer::isReadEof() const {
    return readEOF;
}

bool protocom::PFramer::isReadAgain() const {
    return readAgain;
}

void protocom::PFramer::readAll() {
    while (tryRead());
}
