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
 *  output_opus_enc.cpp
 *  Tested on:
 *  	1. Teensy 4.0
 * 		2. Audio Adaptor Board Rev D
 *
 * Purpose:    Encode audio from Teensy Audio Library to Opus data 
 *
 *******************************************************************************/
#ifndef output_opus_enc_h_
#define output_opus_enc_h_
/*******************************************************************************/

#include "Arduino.h"
#include "AudioStream.h"
#include "opus.h"
#include "config.h"

#include "../common/frame_sink.h"

class AudioOutputOpusEnc : public AudioStream {
public:
    static constexpr size_t OPUS_ENCODER_CHANNEL_COUNT = 2;

    explicit AudioOutputOpusEnc(FrameSink *frameSink);

    void setComplexity(uint8_t complexity);

    void setBitrate(uint32_t bitrate);

    void update() override;

protected:

private:
    static constexpr size_t OPUS_ENCODER_SIZE = 7180;

    FrameSink * const mFrameSink;

    std::array<uint8_t, OPUS_ENCODER_SIZE> mOpusEncoderData{};
    OpusEncoder *mEncoderState;
    audio_block_t *mInputQueueArray[OPUS_ENCODER_CHANNEL_COUNT]{nullptr, nullptr};
    std::array<int16_t, AUDIO_BLOCK_SAMPLES * OPUS_ENCODER_CHANNEL_COUNT> mInputBuffer{};
};

#endif