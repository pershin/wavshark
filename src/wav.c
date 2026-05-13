#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "wav.h"

struct wav_handle_struct {
  FILE *stream;
};

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

  return handle;

cleanup:
  if (NULL != handle) {
    if (NULL != handle->stream)
      fclose(handle->stream);

    free(handle);
  }

  return NULL;
}
