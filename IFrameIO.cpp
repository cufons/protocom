//
// Created by cufon on 11.01.24.
//

#include <cstring>
#include "protocom/IFrameIO.h"
namespace protocom {

    PFrame::PFrame() {
        header = 0;
        len = 0;
        msg = nullptr;
        hasDynamicData = false;
    }

    PFrame::PFrame(const PFrame &other) {

        header = other.header;
        len = other.len;
        msg = new uint8_t[len];
        hasDynamicData = true;
        memcpy((void *) msg, other.msg, len);
    }

    PFrame::~PFrame() {
        if(hasDynamicData) {
            delete[] msg;
        }
    }

    PFrame &PFrame::operator=(const PFrame &other) {
        if(this == &other) return *this;

        header = other.header;
        len = other.len;
        if(hasDynamicData) delete[] msg;
        msg = new uint8_t[len];
        hasDynamicData = true;
        memcpy((void *) msg, other.msg, len);
        return *this;
    }

    PFrame &PFrame::operator=(PFrame &&other)  noexcept {
        header = other.header;
        len = other.len;
        if(hasDynamicData) delete[] msg;
        msg = other.msg;
        hasDynamicData = other.hasDynamicData;
        other.hasDynamicData = false;
        return *this;
    }

    void PFrame::allocMsg() {
        if(hasDynamicData) return;
        msg = new uint8_t[len];
        hasDynamicData = true;
    }

    PFrame::PFrame(PFrame &&other)  noexcept {
        header = other.header;
        len = other.len;
        msg = other.msg;
        hasDynamicData = other.hasDynamicData;
        other.hasDynamicData = false;
    }

    void PFrame::erase() {
        if(hasDynamicData) {
            delete msg;
        }
        header = 0;
        len = 0;
        msg = nullptr;
        hasDynamicData = false;
    }

}