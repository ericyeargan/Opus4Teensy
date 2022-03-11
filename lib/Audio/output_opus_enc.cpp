#include "output_opus_enc.h"

#include <cassert>
#include <memcpy_audio.h>

#include "AudioStream.h"

#include "opus.h"
#include "config.h"

AudioOutputOpusEnc::AudioOutputOpusEnc() : AudioStream(OPUS_ENCODER_CHANNEL_COUNT, mInputQueueArray) {
    assert(static_cast<size_t>(opus_encoder_get_size(OPUS_ENCODER_CHANNEL_COUNT)) == mOpusEncoderData.size());
    mEncoderState = reinterpret_cast<OpusEncoder *>(mOpusEncoderData.data());

    int ret;
    ret = opus_encoder_init(mEncoderState, AUDIO_SAMPLE_RATE, OPUS_ENCODER_CHANNEL_COUNT,
                            OPUS_APPLICATION_AUDIO);
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_BITRATE(CONFIG_OPUS_BITRATE));
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_BANDWIDTH(OPUS_AUTO));
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_COMPLEXITY(CONFIG_OPUS_COMPLEXITY));
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_SIGNAL(OPUS_AUTO));
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_LSB_DEPTH(16));
    assert(ret == OPUS_OK);
    ret = opus_encoder_ctl(mEncoderState, OPUS_SET_VBR(0));
    assert(ret == OPUS_OK);
}

void AudioOutputOpusEnc::setComplexity(uint8_t complexity) {
    auto ret = opus_encoder_ctl(mEncoderState, OPUS_SET_COMPLEXITY(complexity));
    assert(ret == OPUS_OK);
}

void AudioOutputOpusEnc::setBitrate(uint32_t bitrate) {
    auto ret = opus_encoder_ctl(mEncoderState, OPUS_SET_BITRATE(bitrate));
    assert(ret == OPUS_OK);
}

void AudioOutputOpusEnc::update() {
    audio_block_t *leftInput = receiveReadOnly(0);
    audio_block_t *rightInput = receiveReadOnly(1);

    if (leftInput && rightInput) {
        memcpy_tointerleaveLR(mInputBuffer.data(), leftInput->data, rightInput->data);

        mEncodeQueue.writeFrame([this](uint8_t *data) -> size_t {
            auto compressedFrameSize = opus_encode(mEncoderState, mInputBuffer.data(), AUDIO_BLOCK_SAMPLES,
                                                   data,
                                                   static_cast<opus_int32>(EncodeQueue::maxFrameSize()));

            assert(compressedFrameSize > 0);
            assert(static_cast<size_t>(compressedFrameSize) <= EncodeQueue::maxFrameSize());

            return compressedFrameSize;
        });
        release(leftInput);
        release(rightInput);
    }
}
