//
// Created by cufon on 16.01.24.
//


#include "protocom/Client.h"
#include "protocom/pcomserver.pb.h"
#include "protocom/x25519KexProtocol.h"
#include "protocom/EncrMessageCoder.h"

namespace protocom {
    using string = std::basic_string<char>;
    Client::Client(const char *ip, uint16_t port) : remAddr{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip)}
    } , fd(-1) {
        isConnected = false;
        io = nullptr;
        coder = new MessageCoder();
    }

    bool Client::connect_sock(int socketTimeout) {
        if(isConnected) return true;
        fd = socket(AF_INET,SOCK_STREAM,0);
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;

        if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) {
            perror("setsockopt failed");
            return false;
        }

        if (setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout) < 0) {
            perror("setsockopt failed");
            return false;
        }
        if(fd == -1 || ::connect(fd, reinterpret_cast<const sockaddr *>(&remAddr), sizeof(remAddr))) {
            return false;
        }

        io = new IOFrameSocket(fd,socketTimeout);
        isConnected = true;
        return true;
    }

    Client::~Client() {
        delete coder;
        if(isConnected) {
            delete io;
            close(fd);
        }
    }

    bool Client::fetchMsg(MessageLite &msg) {
        PFrame f{};
        std::cout << "Sending: " << msg.DebugString() << std::endl;
        if(!(io->readFrame(f) && coder->decode(f,msg))) return false;
        return true;
    }

    bool Client::sendMsg(const MessageLite &msg) {
        PFrame f{};
        if(!(coder->encode(f,msg) && io->writeFrame(f))) {
            return false;
        }
        return true;
    }

    bool Client::connect() {
        if(!connect_sock(0)) return false;
        ClientConnectedStateRequest req;
        x25519KexProtocol kexProto;
        kexProto.init();
        const SecByteBlock* pubkey = kexProto.getPubKey();
        if(pubkey == nullptr) return false;

        req.set_type(ClientConnectedStateRequest_RequestType_REQUEST_KEX);
        req.mutable_kex()->set_alg(KexMsg_KexAlg_KEX_ECDH);
        req.mutable_kex()->set_pkey(string(reinterpret_cast<const char *>(pubkey->data()), pubkey->size()));
        if(!sendMsg(req))return false;

        ServerConnectedStateResponse resp;
        if(!fetchMsg(resp)) return false;
        if(resp.status() != ServerConnectedStateResponse_ResponseStatus_OK) {
            dump_error(resp);
            return false;
        }
        if(!resp.has_kex()) {
            std::cout << "[Client::connect] Response does not have kex message" << std::endl;
            return false;
        }
        if(resp.kex().alg() != KexMsg_KexAlg_KEX_ECDH || !kexProto.loadOtherKeyArr(reinterpret_cast<const unsigned char *>(resp.kex().pkey().data()), resp.kex().pkey().size())
        || !kexProto.agree()){
            std::cout << "[Client::connect] Failed to load key or agree" << std::endl;
            return false;
        }

        SecByteBlock agreedKey(32);

        if(!kexProto.getKey256(agreedKey)) {
            std::cout << "[Client::connect] Failed to get shared key" << std::endl;
        };
        SecByteBlock iv;
        iv.Assign(reinterpret_cast<const unsigned char *>(resp.iv().data()), resp.iv().length());
        //auto mcoder = new EncrMessageCoder(agreedKey,iv);
        auto mcoder = new EncrMessageCoder(agreedKey);
        delete coder;
        coder = mcoder;
        req.Clear();
        req.set_type(ClientConnectedStateRequest_RequestType_REQUEST_AUTH);
        if(!sendMsg(req))return false;

        if(!fetchMsg(resp)) return false;
        if(resp.status() != ServerConnectedStateResponse_ResponseStatus_OK) {
            dump_error(resp);
            return false;
        }

        isConnected = true;
        return true;
    }

    void Client::dump_error(MessageLite &msg) {
        std::cout << "[Client::dump_error] Server responded with invalid status. Response dump:" << std::endl << msg.DebugString() << std::endl;
    }

    bool Client::fetchRequest(const MessageLite &req, MessageLite &resp) {
        if(!sendMsg(req)) return false;
        if(!fetchMsg(resp)) return false;
        return true;
    }

    bool Client::authenticate(const std::string& username, const std::string& password) {
        ClientAuthRequest authr;
        authr.set_request(ClientAuthRequest_RequestType_AUTH_SUPPLY);
        authr.mutable_username()->append(username);
        authr.mutable_authcredential()->append(password);
        if(!sendMsg(authr))return false;

        ServerAuthResponse authResponse;
        if(!fetchMsg(authResponse)) return false;
        if(authResponse.status() != ServerAuthResponse_ResponseStatus_AUTH_ACCEPT) {
            return false;
        }
        isAuthenticated = true;
        return true;
    }

    bool Client::hasConnected() const {
        return isConnected;
    }

    bool Client::hasAuthenticated() const {
        return isAuthenticated;
    }

    bool Client::hasServerClosed() const {
        return io->isEOF();
    }

} // protocom