//
// Created by cufon on 13.01.24.
//

#ifndef PROTOCOM_X25519KEXPROTOCOL_H
#define PROTOCOM_X25519KEXPROTOCOL_H

#include <cryptopp/xed25519.h>
#include <cryptopp/osrng.h>
#include <cryptopp/sha.h>
namespace protocom {
    using namespace CryptoPP;
    class x25519KexProtocol {
        AutoSeededRandomPool prng;
        x25519 kexObj;
        SecByteBlock privKey;
        SecByteBlock pubKey;
        SecByteBlock pubKeyOther;
        SecByteBlock shared;
        SHA512 kdf;
        bool isInit;
        bool isSharedSecretAgreed;
    public:
        const SecByteBlock *getShared() const;
        const SecByteBlock *getPubKey() const;



        x25519KexProtocol();
        void init();
        bool loadOtherKeyArr(const unsigned char * buf, size_t size);
        void loadOtherKey(const SecByteBlock* otherKey);
        bool agree();
        bool getKey256(SecByteBlock &key);
    };

} // protocom

#endif //PROTOCOM_X25519KEXPROTOCOL_H
