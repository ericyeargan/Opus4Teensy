#pragma once

#include <cstdint>

class RtpCodec {
public:
    static constexpr auto RTP_HEADER_SIZE = 12;

    static void encodeHeader(uint8_t *packetBuffer, uint8_t payloadType, uint32_t timestamp, uint16_t sequenceNumber);

    static bool
    decodeHeader(uint8_t const *packetBuffer, uint8_t *payloadType, uint32_t *timestamp, uint16_t *sequenceNumber);
};
