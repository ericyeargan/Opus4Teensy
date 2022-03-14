#pragma once

#include "frame_sink.h"

class RtpFramer : public FrameSink {
public:
    RtpFramer(FrameSink *delegate, uint8_t payloadType, uint32_t frameDurationSamples);

    size_t getMaxFrameSize() override;

    bool writeFrame(WriteFrameFunction const &writeFunction) override;

private:
    FrameSink * const mDelegate;
    uint8_t const mPayloadType;
    uint32_t const mFrameDuration;

    uint16_t mSequenceNumber{0};
    uint32_t mTimestamp{0};
};
