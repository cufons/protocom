cmake_minimum_required(VERSION 3.25)
project(protocom)

set(CMAKE_CXX_STANDARD 17)
link_libraries(protobuf cryptopp)
add_library(protocom SHARED msg.pb.cc Server.cpp ServerClientHandler.cpp IIOFrame.cpp  IOFrameSocket.cpp ProtocolStateHandler.cpp  ProtocolContext.cpp ProtocolConnectedHandler.cpp  pcomserver.pb.cc MessageCoder.cpp x25519KexProtocol.cpp EncrMessageCoder.cpp Authenticator.cpp ProtocolAuthenticationHandler.cpp TestClient.cpp  ProtocolUserHandler.cpp  ProtocolUserHandlerFactory.cpp  NullUserHandlerFactory.cpp)
link_libraries(protocom)
add_executable(protocom_test main.cpp  testuserprotocol.pb.cc TestUserHandler.cpp  TestUserHandlerFactory.cpp)
