//
// Created by cufon on 16.01.24.
//


#include "TestClient.h"
#include "pcomserver.pb.h"
#include "x25519KexProtocol.h"
#include "EncrMessageCoder.h"

namespace protocom {
    TestClient::TestClient(const char *ip, uint16_t port) : remAddr{
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr = {inet_addr(ip)}
    } , fd(-1) {
        isConnected = false;
        io = nullptr;
        coder = new MessageCoder();;
    }

    bool TestClient::connect_sock(int socketTimeout) {
        if(isConnected) return true;
        fd = socket(AF_INET,SOCK_STREAM,0);
        if(fd == -1 || ::connect(fd, reinterpret_cast<const sockaddr *>(&remAddr), sizeof(remAddr))) {
            return false;
        }

        io = new IOFrameSocket(fd,socketTimeout);
        isConnected = true;
        return true;
    }

    TestClient::~TestClient() {
        delete coder;
        if(isConnected) {
            delete io;
            close(fd);
        }
    }

    bool TestClient::fetchMsg(MessageLite &msg) {
        PFrame f{};
        std::cout << "Sending: " << msg.DebugString() << std::endl;
        if(!(io->readFrame(f) && coder->decode(f,msg))) return false;
        return true;
    }

    bool TestClient::sendMsg(const MessageLite &msg) {
        PFrame f{};
        if(!(coder->encode(f,msg) && io->writeFrame(f)))return false;
        return true;
    }

    bool TestClient::connect() {
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
            std::cout << "[TestClient::connect] Response does not have kex message" << std::endl;
            return false;
        }
        if(resp.kex().alg() != KexMsg_KexAlg_KEX_ECDH || !kexProto.loadOtherKeyArr(reinterpret_cast<const unsigned char *>(resp.kex().pkey().data()), resp.kex().pkey().size())
        || !kexProto.agree()) return false;

        SecByteBlock agreedKey(32);

        if(!kexProto.getKey256(agreedKey)) return false;

        coder = new EncrMessageCoder(agreedKey);

        req.Clear();
        req.set_type(ClientConnectedStateRequest_RequestType_REQUEST_AUTH);
        if(!sendMsg(req))return false;

        if(!fetchMsg(resp)) return false;
        if(resp.status() != ServerConnectedStateResponse_ResponseStatus_OK) {
            dump_error(resp);
            return false;
        }

        ClientAuthRequest authr;
        authr.set_request(ClientAuthRequest_RequestType_AUTH_SUPPLY);
        if(!sendMsg(authr))return false;

        ServerAuthResponse authResponse;
        if(!fetchMsg(authResponse)) return false;
        if(authResponse.status() != ServerAuthResponse_ResponseStatus_AUTH_ACCEPT) {
            dump_error(resp);
            return false;
        }

        return true;
    }

    void TestClient::dump_error(MessageLite &msg) {
        std::cout << "[TestClient::dump_error] Server responded with invalid status. Response dump:" << std::endl << msg.DebugString() << std::endl;
    }

    bool TestClient::fetchRequest(const UserRequest &req, ServerResponse &resp) {
        if(!sendMsg(req)) return false;
        if(!fetchMsg(resp)) return false;
        return true;
    }

} // protocom