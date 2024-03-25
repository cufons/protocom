cmake_minimum_required(VERSION 3.25)
project(protocom)
set(CMAKE_CXX_FLAGS -Wl,--copy-dt-needed-entries)
set(CMAKE_CXX_STANDARD 17)

find_library(PROTOBUF_LIB NAMES protobuf PATHS /usr/local/lib64)
include_directories(host_files)
add_library(protocom STATIC msg.pb.cc Server.cpp ServerClientHandler.cpp IIOFrame.cpp  IOFrameSocket.cpp ProtocolStateHandler.cpp  ProtocolContext.cpp ProtocolConnectedHandler.cpp  pcomserver.pb.cc MessageCoder.cpp x25519KexProtocol.cpp EncrMessageCoder.cpp Authenticator.cpp ProtocolAuthenticationHandler.cpp Client.cpp ProtocolUserHandler.cpp  ProtocolUserHandlerFactory.cpp  NullUserHandlerFactory.cpp)
target_link_libraries(protocom protobuf cryptopp)

add_executable(protocom_test main.cpp TestUserHandler.cpp  TestUserHandlerFactory.cpp testuserprotocol.pb.cc )
target_link_libraries(protocom_test protocom cryptopp ${PROTOBUF_LIB} absl_all)

#target_link_options(protocom_test PRIVATE "LINK:--dynamic-linker ./interpreter_alt.so")
#add_custom_command(TARGET protocom_test POST_BUILD
#        COMMAND  patchelf --set-interpreter "./interpreter_alt.so" protocom_test)