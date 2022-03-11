#include <unity.h>
#include <cstring>

#include "frame_queue.h"

void testStaticMethods() {
    using Queue = FrameQueue<2, 4>;

    TEST_ASSERT_EQUAL(2, Queue::queueLength());
    TEST_ASSERT_EQUAL(4, Queue::maxFrameSize());
}

void testReadReturnsFalseOnEmpty() {
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

void testReadReturnsTrueOnNonEmpty() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    queue.writeFrame([&](uint8_t *data) {
        return 0;
    });

    TEST_ASSERT_TRUE(queue.readFrame([](uint8_t const* data, size_t length) {
    }));
}

void testWriteReturnsTrueOnNonFull() {
    using Queue = FrameQueue<2, 4>;
    Queue queue;

    TEST_ASSERT_TRUE(queue.writeFrame([&](uint8_t *data) {
        return 0;
    }));
}

void testWriteReturnsFalseOnFull() {
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

void testReadCallsFunction() {
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

void testSequentialWriteRead() {
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

void testInterleavedWriteRead() {
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

void process() {
    UNITY_BEGIN();
    RUN_TEST(testStaticMethods);
    RUN_TEST(testReadReturnsFalseOnEmpty);
    RUN_TEST(testReadReturnsTrueOnNonEmpty);
    RUN_TEST(testWriteReturnsTrueOnNonFull);
    RUN_TEST(testWriteReturnsFalseOnFull);
    RUN_TEST(testReadCallsFunction);
    RUN_TEST(testSequentialWriteRead);
    RUN_TEST(testInterleavedWriteRead);
    UNITY_END();
}

int main(int argc, char **argv) {
    process();
    return 0;
}