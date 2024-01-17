#include <iostream>
#include <cstdio>
#include "msg.pb.h"
#include "Server.h"
#include "x25519KexProtocol.h"
#include "EncrMessageCoder.h"
#include "TestClient.h"
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include "TestUserHandlerFactory.h"
#include <chrono>
void runClientTest() {
    protocom::TestClient c("127.0.0.1", 4444);
    if(c.connect()) {
        std::cout << "Connected!!" << std::endl;
    } else {
        std::cout << "Connection failed!!!" << std::endl;
        return;
    }
    UserRequest req;
    ServerResponse resp;
    req.set_msg("Hello there");

    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        if(!c.fetchRequest(req,resp)) {
            std::cout << "User request failed!!!" << std::endl;
            return;
        }
        auto stop = std::chrono::high_resolution_clock::now();
        std::cout << "Server response in " << std::chrono::duration_cast<std::chrono::milliseconds>(stop-start).count() << std::endl;
        std::cout << "Server answer:" << resp.msg() << std::endl;
    }

}
int main(int argc,char** argv) {
    if(argc > 1) {
        if(std::string("clientTest") == argv[1]) {
            runClientTest();
            return 0;
        }
    }
    protocom::Test t;
    t.set_id(324);
    t.set_str("A string");
    int bsize = t.ByteSizeLong();
    char* buf = new char[bsize];

    t.SerializePartialToArray(buf,bsize);
    std::cout << "Encoded message hex dump" << std::endl;
    for(int i = 0; i < bsize; i++) {
        printf("%02x ",buf[i] &0xff);
    }
    std::cout << std::endl;

    CryptoPP::AutoSeededRandomPool prng;
    CryptoPP::SecByteBlock testkey(CryptoPP::AES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock(testkey,testkey.size());

    protocom::EncrMessageCoder coder(testkey);
    protocom::PFrame testframe{};
    coder.encode(testframe,t);
    //*(char*)testframe.msg = 0;
    std::cout << "Encrypted frame hex dump" << std::endl;
    printf("%02x ",testframe.header);
    printf("%02x ",(testframe.len >> 8)&0xFF);
    printf("%02x ",testframe.len&0xFF);
    for (int i = 0; i < testframe.len; ++i) {
        printf("%02x ",testframe.msg[i]);
    }
    std::cout << std::endl;
    protocom::Test decrt;

    if (!coder.decode(testframe,decrt)) {
        std::cout << "Decode failed!" << std::endl;
        return -1;
    }
    std::cout << decrt.DebugString() << std::endl ;
    protocom::x25519KexProtocol protoA,protoB;
    protoA.init();
    protoB.init();
    protoB.loadOtherKey(protoA.getPubKey());
    protoA.loadOtherKey(protoB.getPubKey());
    if(!protoA.agree() || !protoB.agree()) {
        std::cout << "kex agreement failed!" << std::endl;
        return -1;
    }
    CryptoPP::HexEncoder enc(new CryptoPP::FileSink(std::cout));
    CryptoPP::SecByteBlock keya(32),keyb(32);
    if(!protoA.getKey256(keya) || !protoB.getKey256(keyb)) {
        std::cout << "kdf failed" << std::endl;
        return -1;
    }
    std::cout << "shared(A): ";
    CryptoPP::StringSource(keya,keya.size(), true,new CryptoPP::Redirector(enc));
    std::cout << std::endl;

    std::cout << "shared(B): ";
    CryptoPP::StringSource(keyb,keyb.size(), true,new CryptoPP::Redirector(enc));
    std::cout << std::endl;
    protocom::Server s("127.0.0.1",4444);
    s.setUserHandlerFactory(new protocom::TestUserHandlerFactory());
    s.bindSock();
    s.run();
    return 0;
}
