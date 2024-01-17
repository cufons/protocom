//
// Created by cufon on 14.01.24.
//

#ifndef PROTOCOM_ENCRMESSAGECODER_H
#define PROTOCOM_ENCRMESSAGECODER_H

#include "MessageCoder.h"
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#include <cryptopp/hex.h>

namespace protocom {
    using namespace CryptoPP;
    class EncrMessageCoder : public MessageCoder {
        std::vector<uint8_t> msgBuf;
        AutoSeededRandomPool prng;
        GCM<AES>::Decryption decr;
        GCM<AES>::Encryption encr;
        SecByteBlock key;
        SecByteBlock init_iv;
        const size_t ivsize;
        CryptoPP::HexEncoder enc;
    public:
        EncrMessageCoder();
        explicit EncrMessageCoder(SecByteBlock& key);
        bool decode(const PFrame &frame, MessageLite& msg) override;
        bool encode(PFrame& frame, const MessageLite &msg) override;
    };

} // protocom

#endif //PROTOCOM_ENCRMESSAGECODER_H
