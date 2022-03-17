
#include <cassert>
#include <memory>
#include "input_opus_dec.h"
#include "Arduino.h"
#include "AudioStream.h"
#include "opus.h"
#include "config.h"

AudioInputOpusDec::AudioInputOpusDec(FrameSource *frameSource) : AudioStream(1, mOutputQueueArray),
                                                                 mFrameSource(frameSource) {
    size_t decoderStateSize = opus_decoder_get_size(CONFIG_OPUS_CHANNEL_COUNT);
    Serial.printf("\r\nCheck Decoder size %d = 9224", decoderStateSize);
    assert(decoderStateSize <= mOpusDecoderData.size());

    auto ret = opus_decoder_init(mDecoderState, AUDIO_SAMPLE_RATE, CONFIG_OPUS_CHANNEL_COUNT);
    assert(ret == OPUS_OK);
}

// putData() - Returns the time since update() was last called.
// Very useful for phasing the decoder with an asynchronous encoder.
// Minor changes to PLL4 PFD will not impact audio continuity e.g;
// int32_t denominator = CCM_ANALOG_PLL_AUDIO_DENOM;
// ...
// phase = opusDecoder.putData(opusBuffer, opusBufSize);   
// CCM_ANALOG_PLL_AUDIO_DENOM = (denominator + (phase/100) - 100);


void AudioInputOpusDec::update() {
    using UniqueBlockPtr = std::unique_ptr<audio_block_t, std::function<void(audio_block_t*)>>;

    mFrameSource->readFrame([&](uint8_t const *data, size_t length) {
        std::array<UniqueBlockPtr, CONFIG_OPUS_CHANNEL_COUNT> blocks; // NOLINT(cppcoreguidelines-pro-type-member-init)
        for (size_t i = 0; i < CONFIG_OPUS_CHANNEL_COUNT; i++) {
            blocks[i] = {allocate(), [](audio_block_t* block) {
                release(block);
            }};

            if (!blocks[i]) {
                Serial.println("block allocation failed");
                return;
            }
        }

        // TODO: handle odd frame sizes
        auto ret = opus_decode(mDecoderState, data, static_cast<opus_int32>(length), mOutputBuffer.data(),
                               AUDIO_BLOCK_SAMPLES, 0);

        assert(ret > 0);
        assert(ret < AUDIO_BLOCK_SAMPLES);

        for (unsigned channelIndex = 0; channelIndex < CONFIG_OPUS_CHANNEL_COUNT; channelIndex++) {
            for (unsigned sampleIndex = 0; sampleIndex < AUDIO_BLOCK_SAMPLES; sampleIndex++) {
                blocks[channelIndex]->data[sampleIndex] = mOutputBuffer[sampleIndex * CONFIG_OPUS_CHANNEL_COUNT +
                                                                        channelIndex];
            }
        }

        for (size_t i = 0; i < CONFIG_OPUS_CHANNEL_COUNT; i++) {
            transmit(blocks[i].get());
        }
    });
}
