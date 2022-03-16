// Demo code for Opus Encode/Decode for Teensy 4.0
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>

#if AUDIO_BLOCK_SAMPLES != 960
#error Please define AUDIO_BLOCK_SAMPLES 960 and AUDIO_SAMPLE_RATE_EXACT 48000.0f (in AudioStream.h)
#endif

#include <input_opus_dec.h>
#include <output_opus_enc.h>
#include <frame_queue.h>

AudioControlSGTL5000 sgtl5000_1;
AudioInputI2S i2s_in;
AudioOutputI2S i2s_out;

FrameQueue<4, 256> encodedFrameQueue;
AudioOutputOpusEnc opusEncoder(&encodedFrameQueue);  // Create Opus Encoder
AudioInputOpusDec opusDecoder(&encodedFrameQueue);  // Create Opus Decoder
AudioConnection patchCord1(i2s_in, 0, opusEncoder, 0);
AudioConnection patchCord2(opusDecoder, 0, i2s_out, 0);
AudioConnection patchCord3(opusDecoder, 0, i2s_out, 1);

void setup() {
    AudioMemory(10);
    sgtl5000_1.enable();
    sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
    sgtl5000_1.volume(0.5);
    //opusEncoder.setBitrate(16000); // Default is 64000
}

void loop() {
}
