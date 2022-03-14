#include "rtp_codec.h"

#include <cstddef>

static constexpr auto RTP_MAGIC = 0x80; // version(2), Padding(0), Extension(0), CSRC count(0)

void RtpCodec::encodeHeader(uint8_t *packetBuffer, uint8_t payloadType, uint32_t timestamp, uint16_t sequenceNumber) {
    packetBuffer[0] = RTP_MAGIC;
    packetBuffer[1] = payloadType;
    packetBuffer[2] = (sequenceNumber & 0xFF00) >> 8;
    packetBuffer[3] = (sequenceNumber & 0x00FF);
    packetBuffer[4] = (timestamp & 0xFF000000) >> 24;
    packetBuffer[5] = (timestamp & 0x00FF0000) >> 16;
    packetBuffer[6] = (timestamp & 0x0000FF00) >> 8;
    packetBuffer[7] = (timestamp & 0x000000FF);
    packetBuffer[8] = 0x29; // SSRC...
    packetBuffer[9] = 0x13;
    packetBuffer[10] = 0x5d;
    packetBuffer[11] = 0x2e;
}

template<typename T>
static T decodeInt(uint8_t const *buffer) {
    T value{0};

    auto byteCount = sizeof(T);
    for (size_t i = 0; i < byteCount; i++) {
        value |= static_cast<T>(buffer[i] << ((byteCount - i - 1) * 8));
    }
    return value;
}

bool RtpCodec::decodeHeader(const uint8_t *packetBuffer, uint8_t *payloadType, uint32_t *timestamp,
                            uint16_t *sequenceNumber) {
    if (packetBuffer[0] != RTP_MAGIC) {
        return false;
    }

    *payloadType = packetBuffer[1];
    *sequenceNumber = decodeInt<uint16_t>(packetBuffer + 2);
    *timestamp = decodeInt<uint32_t>(packetBuffer + 4);

    auto ssrc = decodeInt<uint32_t>(packetBuffer + 8);

    return true;
}