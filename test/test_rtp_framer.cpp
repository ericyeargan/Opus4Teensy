#include <unity.h>
#include <cstring>
#include <vector>

#include "rtp_framer.h"
#include "rtp_codec.h"

class FakeFrameSink : public FrameSink {
public:
    static constexpr size_t MAX_FRAME_SIZE = 32;

    FakeFrameSink() : mBuffer(MAX_FRAME_SIZE) {
    }

    size_t getMaxFrameSize() override {
        return mBuffer.size();
    }

    bool writeFrame(const WriteFrameFunction &writeFunction) override {
        mWriteCallCount++;
        writeFunction(mBuffer.data());
        return mWriteFrameReturn;
    }

    std::vector<uint8_t> mBuffer;
    bool mWriteFrameReturn{true};
    size_t mWriteCallCount{0};
};

static void testFrameSize() {
    FakeFrameSink delegate;
    RtpFramer framer(&delegate, 0xFF, 4);

    TEST_ASSERT_LESS_THAN(delegate.getMaxFrameSize(), framer.getMaxFrameSize());
}

static void testDelegateWrite() {
    FakeFrameSink delegate;
    RtpFramer framer(&delegate, 0xFF, 4);

    bool written = framer.writeFrame([&](uint8_t *data) {
        return 0;
    });

    TEST_ASSERT_TRUE(written);
    TEST_ASSERT_EQUAL(1, delegate.mWriteCallCount);

    delegate.mWriteFrameReturn = false;

    memset(delegate.mBuffer.data(), 0, delegate.mBuffer.size());

    written = framer.writeFrame([&](uint8_t *data) {
        memset(data, 32, framer.getMaxFrameSize());
        return framer.getMaxFrameSize();
    });

    TEST_ASSERT_FALSE(written);
    TEST_ASSERT_EQUAL(2, delegate.mWriteCallCount);

    for (size_t i = RtpCodec::RTP_HEADER_SIZE; i < delegate.getMaxFrameSize(); i++) {
        TEST_ASSERT_EQUAL(32, delegate.mBuffer[i]);x
    }
}

static void testRtpEncode() {
    FakeFrameSink delegate;
    RtpFramer framer(&delegate, 0xFF, 4);

    uint8_t payloadType;
    uint32_t timestamp;
    uint16_t sequenceNumber;

    framer.writeFrame([&](uint8_t *data) {
        return 0;
    });

    RtpCodec::decodeHeader(delegate.mBuffer.data(), &payloadType, &timestamp, &sequenceNumber);
    TEST_ASSERT_EQUAL(0xFF, payloadType);
    TEST_ASSERT_EQUAL(0, timestamp);
    TEST_ASSERT_EQUAL(0, sequenceNumber);

    framer.writeFrame([&](uint8_t *data) {
        return 0;
    });

    RtpCodec::decodeHeader(delegate.mBuffer.data(), &payloadType, &timestamp, &sequenceNumber);
    TEST_ASSERT_EQUAL(0xFF, payloadType);
    TEST_ASSERT_EQUAL(4, timestamp);
    TEST_ASSERT_EQUAL(1, sequenceNumber);
}

static void testFieldsUpdatedOnFailedWrite() {
    FakeFrameSink delegate;
    RtpFramer framer(&delegate, 0xFF, 4);

    uint8_t payloadType;
    uint32_t timestamp;
    uint16_t sequenceNumber;

    delegate.mWriteFrameReturn = false;
    framer.writeFrame([&](uint8_t *data) {
        return 0;
    });

    delegate.mWriteFrameReturn = true;
    framer.writeFrame([&](uint8_t *data) {
        return 0;
    });

    RtpCodec::decodeHeader(delegate.mBuffer.data(), &payloadType, &timestamp, &sequenceNumber);
    TEST_ASSERT_EQUAL(0xFF, payloadType);
    TEST_ASSERT_EQUAL(4, timestamp);
    TEST_ASSERT_EQUAL(1, sequenceNumber);
}

void runRtpFramerTests() {
    UNITY_BEGIN();
    RUN_TEST(testFrameSize);
    RUN_TEST(testDelegateWrite);
    RUN_TEST(testRtpEncode);
    RUN_TEST(testFieldsUpdatedOnFailedWrite);
    UNITY_END();
}
