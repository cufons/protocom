//
// Created by cufon on 11.01.24.
//

#include <cstdio>
#include <system_error>
#include <sys/socket.h>
#include <netinet/in.h>
#include "protocom/IOFrameSocket.h"

namespace protocom {
    bool IOFrameSocket::sendFrame(PFrame &f) {
        size_t nsent;
        nsent = try_send(&f.header,sizeof f.header);
        if(nsent != sizeof f.header) return false;

        uint16_t sent_len = htons(f.len);
        nsent = try_send(&sent_len,sizeof sent_len);
        if(nsent != sizeof f.len) return false;

        nsent = try_send(f.msg,f.len);
        if(nsent != f.len) return false;
        return true;
    }

    bool IOFrameSocket::recvFrame(PFrame &frame) {
        size_t nrecv = try_recv(&frame.header,1);
        if (!nrecv || ((frame.header & 0xF0) != 0xF0)) {
            return false;
        }
        if (!try_recv(&frame.len, 2)) {
            return false;
        }
        frame.len = ntohs(frame.len);

        msgBuf.resize(frame.len);
        size_t to_recv = frame.len;
        while (to_recv) {
            nrecv = try_recv(msgBuf.data() + frame.len - to_recv, to_recv);
            if(!nrecv) return false;
            to_recv -= nrecv;
        }
        if (nrecv != frame.len) {
            return false;
        }
        frame.msg = msgBuf.data();
        return true;
    }

    IOFrameSocket::IOFrameSocket(int sockfd,int timeout) : sockfd(sockfd) {
        conn_open = true;
        if(timeout > 0) {
            setTimeout(timeout);
        }
    }

    size_t IOFrameSocket::try_recv(void *buf, size_t n) {
        if(!conn_open || n == 0) return 0;
        ssize_t nrecv = recv(sockfd, buf, n, 0);
        if(!nrecv) conn_open = false;
        if (nrecv == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            perror("[ServerClientHandler::try_recv] recv failed");
            throw std::system_error(errno, std::generic_category());
        }
        return nrecv;
    }

    size_t IOFrameSocket::try_send(const void *buf, size_t n) {
        if(!conn_open || n == 0) return 0;
        size_t nsent = send(sockfd,buf,n,0);
        if(!nsent) conn_open = false;
        if(nsent == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                return 0;
            }
            perror("[ServerClientHandler::try_send] send failed");
            throw std::system_error(errno, std::generic_category());
        }
        return nsent;
    }

    bool IOFrameSocket::readFrame(PFrame &f) {
        return recvFrame(f);
    }

    bool IOFrameSocket::writeFrame(PFrame &f) {
        return sendFrame(f);
    }

    void IOFrameSocket::setTimeout(int seconds) const {
        timeval timeout = {
                .tv_sec = seconds,
                .tv_usec = 0

        };
        if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&timeout,sizeof timeout) < -1) {
            throw std::system_error(errno, std::generic_category());
        }
        if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof timeout) < -1) {
            throw std::system_error(errno, std::generic_category());
        }
    }

    bool IOFrameSocket::isEOF() {
        return !conn_open;
    }
} // protocom