/******************************************************************************
 * Opus4Teensy Library
 * Copyright (c) 2021, Mick Gergos https://github.com/mgergos/Opus4Teensy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************
 *  input_opus_dec.cpp
 *  Tested on:
 *  	1. Teensy 4.0
 * 		2. Audio Adaptor Board Rev D
 *
 * Purpose:    Decode Opus encoded data for use by Teensy Audio Library
 *
 *******************************************************************************/
#ifndef input_opus_dec_h_
#define input_opus_dec_h_
/*******************************************************************************/

#include "Arduino.h"
#include "AudioStream.h"
#include "opus.h"
#include "config.h"

#include "frame_source.h"

class AudioInputOpusDec : public AudioStream {
public:
    explicit AudioInputOpusDec(FrameSource *frameSource);

    void update() override;

private:
    static constexpr size_t OPUS_DECODER_SIZE = 9224;

    std::array<uint8_t, OPUS_DECODER_SIZE> mOpusDecoderData{};
    OpusDecoder *const mDecoderState{reinterpret_cast<OpusDecoder *>(mOpusDecoderData.data())};

    audio_block_t *mOutputQueueArray[CONFIG_OPUS_CHANNEL_COUNT]{
            nullptr,
#if CONFIG_OPUS_CHANNEL_COUNT > 1
            nullptr
#endif
    };

    std::array<int16_t, AUDIO_BLOCK_SAMPLES * CONFIG_OPUS_CHANNEL_COUNT> mOutputBuffer{};

    FrameSource *const mFrameSource;
};

#endif