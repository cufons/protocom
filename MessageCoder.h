//
// Created by cufon on 13.01.24.
//

#ifndef PROTOCOM_MESSAGECODER_H
#define PROTOCOM_MESSAGECODER_H

#include <google/protobuf/message_lite.h>
#include "IIOFrame.h"

using namespace google::protobuf;
namespace protocom {

    class MessageCoder {
        std::vector<uint8_t> frame_bytes;
    public:
        virtual bool decode(const PFrame &frame, MessageLite& msg);
        virtual bool encode(PFrame& frame, const MessageLite &msg);
    };

} // protocom

#endif //PROTOCOM_MESSAGECODER_H
