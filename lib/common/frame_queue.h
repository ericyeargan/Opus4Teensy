#pragma once

#include <cstdint>
#include <functional>
#include <array>

#include "frame_sink.h"
#include "frame_source.h"

template<std::size_t QueueLength, std::size_t MaxFrameSize>
class FrameQueue : public FrameSource, public FrameSink{
public:
    static size_t queueLength() { return QueueLength; }

    int available() {
        return mAvailable;
    }

    using ReadFrameFunction = std::function<void(uint8_t const *data, size_t length)>;

    bool readFrame(ReadFrameFunction const &readFunction) override {
        if (mAvailable == 0) {
            return false;
        }

        readFunction(mReadIter->mData.data(), mReadIter->mFrameSize);
        if (++mReadIter == mQueue.end()) {
            mReadIter = mQueue.begin();
        }
        mAvailable--;

        return true;
    }

    bool writeFrame(WriteFrameFunction const &writeFunction) override {
        if (mAvailable >= mQueue.size()) {
            return false;
        }

        mWriteIter->mFrameSize = writeFunction(mWriteIter->mData.data());
        if (++mWriteIter == mQueue.end()) {
            mWriteIter = mQueue.begin();
        }
        mAvailable++;

        return true;
    }

    size_t getMaxFrameSize() override { return MaxFrameSize; }

private:
    using Buffer = std::array<uint8_t, MaxFrameSize>;
    struct Frame {
        Buffer mData;
        size_t mFrameSize{0};
    };

    using Queue = std::array<Frame, QueueLength>;
    Queue mQueue;

    typename Queue::iterator mReadIter{mQueue.begin()};
    typename Queue::iterator mWriteIter{mQueue.begin()};

    size_t mAvailable{0};
};
