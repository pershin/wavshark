#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "wav.h"

struct wav_handle_struct {
  FILE *stream;
};

#define RIFF_MARKER 0x46464952 /* RIFF */
#define WAVE_MARKER 0x45564157 /* WAVE */

#pragma pack(push, 1)

typedef struct {
  uint32_t id;
  uint32_t size;
} wav_chunk;

#pragma pack(pop)

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

  return true;
}
