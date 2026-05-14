#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "wav.h"

struct wav_handle_struct {
  FILE *stream;
  uint32_t size;
};

#define RIFF_MARKER 0x46464952 /* RIFF */
#define WAVE_MARKER 0x45564157 /* WAVE */
#define FMT_MARKER 0x20746d66  /* fmt  */
#define DATA_MARKER 0x61746164 /* data */

#pragma pack(push, 1)

typedef struct {
  uint32_t id;
  uint32_t size;
} wav_chunk;

#pragma pack(pop)

static bool wav_find_chunk(FILE *stream, uint32_t id, wav_chunk *chunk);
static bool wav_read_header(wav_handle *handle);

wav_handle *wav_open(const char *filename) {
  wav_handle *handle = NULL;

  handle = calloc(1, sizeof(wav_handle));
  if (NULL == handle) {
    fprintf(stderr, PACKAGE_NAME ": Cannot allocate memory\n");
    goto cleanup;
  }

  handle->stream = fopen(filename, "rb");
  if (NULL == handle->stream) {
    fprintf(stderr, PACKAGE_NAME ": Cannot open file '%s': %s\n", filename,
            strerror(errno));
    goto cleanup;
  }

  if (wav_read_header(handle) == false) {
    fprintf(stderr, PACKAGE_NAME ": Failed to read WAV header from '%s'\n",
            filename);
    goto cleanup;
  }

  return handle;

cleanup:
  if (NULL != handle) {
    if (NULL != handle->stream)
      fclose(handle->stream);

    free(handle);
  }

  return NULL;
}

void wav_close(wav_handle *handle) {
  if (NULL != handle) {
    if (NULL != handle->stream)
      fclose(handle->stream);

    free(handle);
  }
}

static bool wav_find_chunk(FILE *stream, uint32_t id, wav_chunk *chunk) {
  /* Skip RIFF header. */
  if (fseek(stream, sizeof(wav_chunk) + sizeof(uint32_t), SEEK_SET) != 0)
    return false;

  while (fread(chunk, sizeof(wav_chunk), 1, stream) == 1) {
    if (0 == chunk->size)
      return false;

    if (id == chunk->id)
      return true;

    /* Skip chunk data (aligned to 2 bytes). */
    if (fseek(stream, (chunk->size + 1) & ~1, SEEK_CUR) != 0)
      return false;
  }

  return false;
}

static bool wav_read_header(wav_handle *handle) {
  wav_chunk chunk = {0};
  uint32_t type = 0;

  if (fread(&chunk, sizeof(wav_chunk), 1, handle->stream) != 1)
    return false;

  if (RIFF_MARKER != chunk.id)
    return false;

  if (fread(&type, sizeof(uint32_t), 1, handle->stream) != 1)
    return false;

  if (WAVE_MARKER != type)
    return false;

  if (wav_find_chunk(handle->stream, FMT_MARKER, &chunk) == false)
    return false;

  if (wav_find_chunk(handle->stream, DATA_MARKER, &chunk) == false)
    return false;

  handle->size = chunk.size;

  return true;
}
