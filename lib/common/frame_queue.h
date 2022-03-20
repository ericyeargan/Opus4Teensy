#pragma once

#include <cstdint>
#include <functional>
#include <array>
#include <atomic>
#include <cassert>
#include <vector>

#include "frame_sink.h"
#include "frame_source.h"

template<std::size_t QueueLength, std::size_t MaxFrameSize>
class FrameQueue : public ConsumableFrameSource, public FrameSink{
public:
    FrameQueue() {
        assert(mAvailable.is_lock_free());
    }

    static size_t queueLength() { return QueueLength; }

    size_t available() override {
        return mAvailable;
    }

    void registerFrameAvailableCallback(std::function<void()> function) override {
        mAvailableCallbacks.push_back(function);
    }

    bool peekFrame(const ReadFrameFunction &readFunction) const override {
        if (mAvailable == 0) {
            return false;
        }

        readFunction(mReadIter->mData.data(), mReadIter->mFrameSize);
        return true;
    }

    bool readFrame(ReadFrameFunction const &readFunction) override {
        if (!peekFrame(readFunction)) {
            return false;
        }

        auto consumed = consumeFrame();
        assert(consumed);

        return true;
    }

    bool consumeFrame() override {
        if (mAvailable == 0) {
            return false;
        }

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

        bool wasEmpty = mAvailable == 0;

        mWriteIter->mFrameSize = writeFunction(mWriteIter->mData.data());
        if (++mWriteIter == mQueue.end()) {
            mWriteIter = mQueue.begin();
        }
        mAvailable++;

        if (wasEmpty) {
            for (auto const& availableCallback : mAvailableCallbacks) {
                availableCallback();
            }
        }

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

    std::atomic<size_t> mAvailable{0};

    std::vector<FrameAvailableCallback> mAvailableCallbacks;
};
