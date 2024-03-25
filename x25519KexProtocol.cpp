//
// Created by cufon on 13.01.24.
//

#include "protocom/x25519KexProtocol.h"
namespace protocom {
    x25519KexProtocol::x25519KexProtocol() : privKey(x25519::SECRET_KEYLENGTH), pubKey(x25519::PUBLIC_KEYLENGTH), pubKeyOther(0),
                                             shared(x25519::SHARED_KEYLENGTH),isInit(false) {

    }

    void x25519KexProtocol::init() {
        kexObj.GenerateKeyPair(prng,privKey,pubKey);
        isInit = true;
    }

    const SecByteBlock *x25519KexProtocol::getPubKey() const {
        if(!isInit) return nullptr;
        return &pubKey;
    }

    bool x25519KexProtocol::loadOtherKeyArr(const unsigned char *buf, size_t size) {
        if(size != x25519::PUBLIC_KEYLENGTH) return false;
        pubKeyOther.Assign(buf,size);
        return true;
    }

    bool x25519KexProtocol::agree() {
        if(!kexObj.Agree(shared,privKey,pubKeyOther)) return false;
        isSharedSecretAgreed = true;
        return true;
    }

    const SecByteBlock *x25519KexProtocol::getShared() const {
        if(!isSharedSecretAgreed) return nullptr;
        return &shared;
    }


    void x25519KexProtocol::loadOtherKey(const SecByteBlock *otherKey) {
        if(otherKey == nullptr) return;
        pubKeyOther = *otherKey;
    }

    bool x25519KexProtocol::getKey256(SecByteBlock &key) {
        if(!isSharedSecretAgreed || key.size() != 32) return false;
        kdf.Update(shared.data(),shared.size());
        kdf.TruncatedFinal(key.data(),key.size());
        return true;
    }
} // protocom