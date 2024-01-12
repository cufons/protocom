cmake_minimum_required(VERSION 3.25)
project(protocom)

set(CMAKE_CXX_STANDARD 17)

link_libraries(protobuf)
add_executable(protocom main.cpp msg.pb.cc Server.cpp  ClientHandler.cpp  IIOFrame.cpp  IOFrameSocket.cpp ProtocolStateHandler.cpp ProtocolStateHandler.h)
