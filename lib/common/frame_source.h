#pragma once

#include <functional>

class FrameSource {
public:
    // data is non-const to allow passing directly to RadioLib functions
    using ReadFrameFunction = std::function<void(uint8_t *data, size_t length)>;

    virtual bool readFrame(ReadFrameFunction const &readFunction) = 0;

    virtual size_t available() = 0;
};

class ConsumableFrameSource : public FrameSource {
public:
    using FrameAvailableCallback = std::function<void()>;
    virtual void registerFrameAvailableCallback(FrameAvailableCallback) = 0;

    virtual bool peekFrame(ReadFrameFunction const &readFunction) const = 0;

    // has same effect as calling FrameSource::readFrame with a no-op callback
    virtual bool consumeFrame() = 0;
};
