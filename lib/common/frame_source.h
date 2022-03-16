#pragma once

#include <functional>

class FrameSource {
public:
    using ReadFrameFunction = std::function<void(uint8_t const *data, size_t length)>;

    virtual bool readFrame(ReadFrameFunction const &readFunction) = 0;
};