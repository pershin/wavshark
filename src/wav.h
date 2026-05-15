#ifndef WAV_H
#define WAV_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct wav_handle_struct;

typedef struct wav_handle_struct wav_handle;

#pragma pack(push, 1)

typedef struct {
  uint16_t format_tag;
  uint16_t channels;
  uint32_t samples_per_sec;
  uint32_t avg_bytes_per_sec;
  uint16_t block_align;
  uint16_t bits_per_sample;
} wav_format;

#pragma pack(pop)

typedef enum { WAV_ERROR = -1, WAV_OK = 0 } wav_errors;

wav_handle *wav_open(const char *filename);
wav_errors wav_get_format(wav_handle *handle, wav_format *format);
size_t wav_read(wav_handle *handle, void *buffer, size_t size);
void wav_close(wav_handle *handle);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: WAV_H */
