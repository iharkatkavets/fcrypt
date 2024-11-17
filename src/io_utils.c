/* io_utils.c */

#include "io_utils.h"

uint8_t* read_input_safe(char *prompt, size_t *out_size) {
  char in_buf[256];
  uint8_t *out_buf;
  size_t in_size;
  struct termios prev_params, new_params;

  if (tcgetattr(fileno(stdin), &prev_params) != 0) {
    fprintf(stderr, "Fail to read termious state: %s\n", strerror(errno));
    return NULL;
  }

  new_params = prev_params;
  new_params.c_lflag &= ~ECHO;

  if (tcsetattr(fileno(stdin), TCSAFLUSH, &new_params) != 0) {
    fprintf(stderr, "Fail to turn off echo: %s\n", strerror(errno));
    return NULL;
  }

  printf("%s", prompt);
  fflush(stdout);

  in_size = read(STDIN_FILENO, in_buf, sizeof(in_buf));
  if (in_size < 0) {
    fprintf(stderr, "Failed to read key: %s\n", strerror(errno));
    return NULL;
  }

  if (in_buf[in_size-1] == '\n') {
    in_size--;
  }

  if (in_size < 1) {
    fprintf(stderr, "No key provided\n");
    return NULL;
  }

  if (!(out_buf = (uint8_t *)malloc(in_size))) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  memcpy(out_buf, in_buf, in_size);

  tcsetattr(fileno(stdin), TCSAFLUSH, &prev_params);

  *out_size = in_size;
  return out_buf;
}
