#ifndef WAV_H
#define WAV_H

#ifdef __cplusplus
extern "C" {
#endif

struct wav_handle_struct;

typedef struct wav_handle_struct wav_handle;

wav_handle *wav_open(const char *filename);

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: WAV_H */
