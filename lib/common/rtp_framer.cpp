#include "rtp_framer.h"
#include "rtp_codec.h"

RtpFramer::RtpFramer(FrameSink *delegate, uint8_t payloadType, uint32_t frameDurationSamples)
        : mDelegate(delegate), mPayloadType(payloadType), mFrameDuration(frameDurationSamples) {
}

bool RtpFramer::writeFrame(const FrameSink::WriteFrameFunction &writeFunction) {
    bool written = mDelegate->writeFrame([&](uint8_t *data) {
        auto payloadSize = writeFunction(data + RtpCodec::RTP_HEADER_SIZE);
        RtpCodec::encodeHeader(data, mPayloadType, mTimestamp, mSequenceNumber);
        return payloadSize + RtpCodec::RTP_HEADER_SIZE;
    });

    mSequenceNumber++;
    mTimestamp += mFrameDuration;

    return written;
}

size_t RtpFramer::getMaxFrameSize() {
    return mDelegate->getMaxFrameSize() - RtpCodec::RTP_HEADER_SIZE;
}
