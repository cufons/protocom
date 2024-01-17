//
// Created by cufon on 13.01.24.
//

#include "MessageCoder.h"

namespace protocom {
    bool MessageCoder::decode(const PFrame &frame, MessageLite &msg) {
        return msg.ParseFromArray(frame.msg,frame.len);
    }

    bool MessageCoder::encode(PFrame &frame, const MessageLite &msg) {
        if(msg.ByteSizeLong() > 65535) return false;
        frame_bytes.resize(msg.GetCachedSize());
        if(!msg.SerializeToArray(frame_bytes.data(),frame_bytes.size())) {
            return false;
        }
        frame.header = 0xF0;
        frame.len = msg.GetCachedSize();
        frame.msg = frame_bytes.data();
        return true;
    }
} // protocom