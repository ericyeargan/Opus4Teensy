#pragma once

#include <cstdint>
#include <functional>
#include <array>

template<std::size_t QueueLength, std::size_t MaxFrameSize>
class FrameQueue {
public:
    static size_t maxFrameSize() { return MaxFrameSize; }

    static size_t queueLength() { return QueueLength; }

    int available() {
        return mAvailable;
    }

    using ReadFrameFunction = std::function<void(uint8_t const *data, size_t length)>;

    bool readFrame(ReadFrameFunction const &readFunction) {
        if (mAvailable == 0) {
            return false;
        }

        readFunction(mQueue[mReadIndex].mData.data(), mQueue[mReadIndex].mFrameSize);
        mReadIndex++;
        if (mReadIndex == mQueue.size()) {
            mReadIndex = 0;
        }
        mAvailable--;

        return true;
    }

    /*!
     * Writes a frame to buffer data and returns the frame size
     */
    using WriteFrameFunction = std::function<size_t(uint8_t *data)>;

    bool writeFrame(WriteFrameFunction const &writeFunction) {
        if (mAvailable >= mQueue.size()) {
            return false;
        }

        mQueue[mWriteIndex].mFrameSize = writeFunction(mQueue[mWriteIndex].mData.data());
        mWriteIndex++;
        if (mWriteIndex == mQueue.size()) {
            mWriteIndex = 0;
        }
        mAvailable++;

        return true;
    }

private:
    using Buffer = std::array<uint8_t, MaxFrameSize>;
    struct Frame {
        Buffer mData;
        size_t mFrameSize{0};
    };

    std::array<Frame, QueueLength> mQueue;
    size_t mAvailable{0};
    size_t mReadIndex{0};
    size_t mWriteIndex{0};
};
