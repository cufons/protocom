//
// Created by cufon on 11.01.24.
//

#ifndef PROTOCOM_IOFRAMESOCKET_H
#define PROTOCOM_IOFRAMESOCKET_H

#include <vector>
#include "IFrameIO.h"

namespace protocom {

    class IOFrameSocket : public IFrameIO {
        int sockfd;
        bool conn_open;
        bool read_eagain;
        bool write_eagain;
        std::vector<uint8_t> msgBuf;
        size_t try_recv(void* buf,size_t n);
        size_t try_send(const void* buf,size_t n);
    public:
        explicit IOFrameSocket(int sockfd,int timeout = 0);
        bool sendFrame(PFrame &f);
        bool recvFrame(PFrame &f);

        bool readFrame(PFrame &f) override;
        bool writeFrame(PFrame &f)override;

        bool isEOF() override;
        void setTimeout(int seconds) const;

        bool isWriteExhausted() override;

        bool isReadExhausted() override;
    };

} // protocom

#endif //PROTOCOM_IOFRAMESOCKET_H
