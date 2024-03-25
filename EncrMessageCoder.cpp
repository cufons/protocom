//
// Created by cufon on 14.01.24.
//

#include <cryptopp/files.h>
#include "protocom/EncrMessageCoder.h"

namespace protocom {
    EncrMessageCoder::EncrMessageCoder(SecByteBlock& key) :  EncrMessageCoder() {
        this->key = key;
        init_iv.resize(ivsize);
        prng.GenerateBlock(init_iv,ivsize);
        encr.SetKeyWithIV(key,key.size(),init_iv);
        decr.SetKeyWithIV(key,key.size(),init_iv);
    }

    EncrMessageCoder::EncrMessageCoder():ivsize(AES::BLOCKSIZE) , enc(new CryptoPP::FileSink(std::cout)) {

    }

    bool EncrMessageCoder::decode(const PFrame &frame, MessageLite &msg) {

        if(frame.len <= ivsize || frame.header != 0xF1) {
            // Will not decode zero length message
            return false;
        }
        const byte* iv = frame.msg;
        //decr.SetKeyWithIV(key,key.size(),iv,ivsize);
        decr.Restart();
        decr.Resynchronize(iv,ivsize);
        const byte* ciphertext = frame.msg + ivsize;
        size_t cipherlen = frame.len - ivsize;
        AuthenticatedDecryptionFilter df(decr);
        try {
            if (df.PutMessageEnd(ciphertext, cipherlen)) {
                std::cout << "[EncrMessageCoder::decode] putMessageEnd failed!" << std::endl;
                return false;
            }
            msgBuf.clear();
            msgBuf.resize(df.TotalBytesRetrievable());
            df.Get(msgBuf.data(), msgBuf.size());
        }
        catch (Exception& e) {
            std::cout << "[EncrMessageCoder::decode] " << e.what() << std::endl;
            return false;
        }
        PFrame decoded_frame = {
                .header = 0xF0,
                .len = static_cast<uint16_t>(msgBuf.size()),
                .msg = msgBuf.data()
        };
        return MessageCoder::decode(decoded_frame, msg);
    }

    bool EncrMessageCoder::encode(PFrame &frame, const MessageLite &msg) {
        struct PFrame plaintextFrame{};
        if(!MessageCoder::encode(plaintextFrame, msg)) return false;
        msgBuf.clear();
        msgBuf.resize(ivsize);
        prng.GenerateBlock(msgBuf.data(),ivsize);
        encr.Restart();
        encr.Resynchronize(msgBuf.data(),ivsize);
        AuthenticatedEncryptionFilter ef(encr);
        try {
            if (ef.PutMessageEnd(plaintextFrame.msg, plaintextFrame.len)) {
                std::cout << "[EncrMessageCoder::encode] putMessageEnd failed!" << std::endl;
                return false;
            }
            msgBuf.resize(ivsize + ef.TotalBytesRetrievable());
            ef.Get(msgBuf.data() + ivsize, ef.TotalBytesRetrievable());
        }
        catch (Exception& e) {
            std::cout << "[EncrMessageCoder::encode] " << e.what() << std::endl;
            return false;
        }
        frame.header = 0xF1;
        frame.len = msgBuf.size();
        frame.msg = msgBuf.data();
        return true;
    }
} // protocom