#pragma once

#include <cstdint>
#include <functional>

class FrameSink {
public:
    virtual size_t getMaxFrameSize() = 0;

    /*!
     * Writes a frame to buffer data and returns the frame size
     */
    using WriteFrameFunction = std::function<size_t(uint8_t *data)>;

    virtual bool writeFrame(WriteFrameFunction const &writeFunction) = 0;
};