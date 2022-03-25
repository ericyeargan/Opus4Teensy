// Demo code for Opus Encode/Decode for Teensy 4.0
#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>

#if AUDIO_BLOCK_SAMPLES != 960
#error Please define AUDIO_BLOCK_SAMPLES 960 and AUDIO_SAMPLE_RATE_EXACT 48000.0f (in AudioStream.h)
#endif

#include <input_opus_dec.h>
#include <output_opus_enc.h>

AudioSynthWaveformSine  sig_gen;
AudioOutputI2S          i2s_out;
AudioOutputOpusEnc      opusEncoder;  // Create Opus Encoder
AudioInputOpusDec       opusDecoder;  // Create Opus Decoder
AudioConnection         patchCord1(sig_gen, 0, opusEncoder, 0);
AudioConnection         patchCord2(opusDecoder, 0, i2s_out, 0);
AudioConnection         patchCord3(opusDecoder, 0, i2s_out, 1);

void setup()
{  
  AudioMemory(10);

  sig_gen.amplitude(0.01f);
  sig_gen.frequency(440.0F);

  opusEncoder.initialise();
  //opusEncoder.setBitrate(16000); // Default is 64000
  opusDecoder.initialise();
}

void loop()
{
  uint8_t *opusBuffer;
  int32_t opusBufSize;

  opusBufSize = opusEncoder.hasData();            // Wait for a full block of Opus data

  if(opusBufSize > 0)
  {
    opusBuffer = opusEncoder.getData();           // Write the encoded data to the buffer
    opusDecoder.putData(opusBuffer, opusBufSize); // Write the buffer to the decoder (loopback)
  }
}
