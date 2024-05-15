#ifndef AUDIO_H
#define AUDIO_H

#include "types.h"

#define DSP_MIXER 0x224
#define DSP_MIXER_DATA 0x225
#define DSP_RESET 0x226
#define DSP_READ 0x22A
#define DSP_WRITE 0x22C
#define DSP_READ_BUFFER_STATUS 0x22E

void dsp_handler();
int32_t dsp_write(int32_t fd, const void *buf, int32_t nbytes);
int32_t dsp_read(int32_t fd, void *buf, int32_t nbytes);
int32_t dsp_open(const uint8_t *filename);
int32_t dsp_close(int32_t fd);
void wait(float time);


typedef struct __attribute__((packed)) {
    char ChunkID[4];        // "RIFF"
    uint32_t ChunkSize;     // File size - 8
    char Format[4];         // "WAVE"
    char Subchunk1ID[4];    // "fmt "
    uint32_t Subchunk1Size; // Format size
    uint16_t AudioFormat;   // Audio format (1 for PCM)
    uint16_t NumChannels;   // Number of channels
    uint32_t SampleRate;    // Sample rate in Hz
    uint32_t ByteRate;      // Byte rate
    uint16_t BlockAlign;    // Block align
    uint16_t BitsPerSample; // Bits per sample
    char Subchunk2ID[4];    // "data"
    uint32_t Subchunk2Size; // Data size
} WAVHeader;

#endif
