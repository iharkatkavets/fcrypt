/* io_utils.c */

#include "io_utils.h"
#include <stddef.h>
#include <stdint.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

size_t read_input_safe(char *prompt, uint8_t *out_buf, size_t out_buf_size) {
  size_t read_size;
  struct termios prev_params, new_params;

  if (tcgetattr(fileno(stdin), &prev_params) != 0) {
    fprintf(stderr, "\nFail to read termious state: %s", strerror(errno));
    return -1;
  }

  new_params = prev_params;
  new_params.c_lflag &= ~ECHO;

  if (tcsetattr(fileno(stdin), TCSAFLUSH, &new_params) != 0) {
    fprintf(stderr, "\nFail to turn off echo: %s", strerror(errno));
    return -1;
  }

  fprintf(stderr, "%s", prompt);
  fflush(stderr);

  read_size = read(STDIN_FILENO, out_buf, out_buf_size);
  if (tcsetattr(fileno(stdin), TCSAFLUSH, &prev_params) != 0) {
    fprintf(stderr, "\nFailed to restore terminal settings: %s", strerror(errno));
  }

  if (read_size < 1) {
    fprintf(stderr, "\nFailed to read input: %s", strerror(errno));
    return -1;
  }

  if (read_size == 0) {
    fprintf(stderr, "\nNo input provided.");
    return -1;
  }

  if (out_buf[read_size-1] == '\n') {
    read_size--;
  }

  if (read_size < 1) {
    fprintf(stderr, "\nNo key provided");
    return -1;
  }

  fprintf(stderr, "\n");

  return read_size;
}
