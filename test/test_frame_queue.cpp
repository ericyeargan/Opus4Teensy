#include <unity.h>
#include <unity_fixture_stubs.h>
#include <cstring>

#include "frame_queue.h"

static void testQueueLength() {
    using Queue = FrameQueue<2, 4>;

    TEST_ASSERT_EQUAL(2, Queue::queueLength());
}

static void testFrameSize() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    TEST_ASSERT_EQUAL(4, queue.getMaxFrameSize());
}

static void testReadReturnsFalseOnEmpty() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    TEST_ASSERT_FALSE(queue.readFrame([](uint8_t const* data, size_t length) {
        TEST_FAIL();
    }));

    TEST_ASSERT_TRUE(queue.writeFrame([&](uint8_t *data) {
        return 0;
    }));

    TEST_ASSERT_TRUE(queue.readFrame([](uint8_t const* data, size_t length) {
    }));
}

static void testReadReturnsTrueOnNonEmpty() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    queue.writeFrame([&](uint8_t *data) {
        return 0;
    });

    TEST_ASSERT_TRUE(queue.readFrame([](uint8_t const* data, size_t length) {
    }));
}

static void testWriteReturnsTrueOnNonFull() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    TEST_ASSERT_TRUE(queue.writeFrame([&](uint8_t *data) {
        return 0;
    }));
}

static void testWriteReturnsFalseOnFull() {
    using Queue = FrameQueue<1, 4>;
    Queue queue;

    queue.writeFrame([&](uint8_t *data) {
        return 0;
    });

    TEST_ASSERT_FALSE(queue.writeFrame([&](uint8_t *data) {
        TEST_FAIL();
        return 0;
    }));
}

static void testReadCallsFunction() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    queue.writeFrame([&](uint8_t *data) {
        return 0;
    });

    bool called{false};
    queue.readFrame([&](uint8_t const* data, size_t length) {
        called = true;
    });

    TEST_ASSERT_TRUE(called);
}

static void testSequentialWriteRead() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    for (size_t c = 0; c < 2; c++) {
        for (size_t frameIndex = 0; frameIndex < Queue::queueLength(); frameIndex++) {
            queue.writeFrame([&](uint8_t *data) {
                memset(data, static_cast<int>(frameIndex), frameIndex);
                return frameIndex;
            });
        }

        for (size_t frameIndex = 0; frameIndex < Queue::queueLength(); frameIndex++) {
            queue.readFrame([&](uint8_t const *data, size_t length) {
                TEST_ASSERT_EQUAL(frameIndex, length);
                for (size_t i = 0; i < length; i++) {
                    TEST_ASSERT_EQUAL(frameIndex, data[i]);
                }
            });
        }
    }
}

static void testInterleavedWriteRead() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    for (size_t frameIndex = 0; frameIndex < 2 * Queue::queueLength(); frameIndex++) {
        queue.writeFrame([&](uint8_t *data) {
            memset(data, static_cast<int>(frameIndex), frameIndex);
            return frameIndex;
        });

        queue.readFrame([&](uint8_t const *data, size_t length) {
            TEST_ASSERT_EQUAL(frameIndex, length);
            for (size_t i = 0; i < length; i++) {
                TEST_ASSERT_EQUAL(frameIndex, data[i]);
            }
        });
    }
}

void runFrameQueueTests() {
    UNITY_BEGIN();
    RUN_TEST(testQueueLength);
    RUN_TEST(testFrameSize);
    RUN_TEST(testReadReturnsFalseOnEmpty);
    RUN_TEST(testReadReturnsTrueOnNonEmpty);
    RUN_TEST(testWriteReturnsTrueOnNonFull);
    RUN_TEST(testWriteReturnsFalseOnFull);
    RUN_TEST(testReadCallsFunction);
    RUN_TEST(testSequentialWriteRead);
    RUN_TEST(testInterleavedWriteRead);
    UNITY_END();
}
