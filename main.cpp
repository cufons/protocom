#include <iostream>
#include <cstdio>
#include "msg.pb.h"
#include "Server.h"

int main() {
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
    protocom::Server s("127.0.0.1",4444);
    s.bindSock();
    s.run();
    return 0;
}
