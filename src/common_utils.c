/* main.c */

#include "common_utils.h"

#include <errno.h>
#include <unistd.h>


int write_to_file(int outfd, uint8_t *buf, ssize_t to_write) {
  ssize_t chunk_written, total_written = 0;

  while (total_written < to_write) {
    chunk_written = write(outfd, buf+total_written, to_write-total_written);
    if (chunk_written < 0) {
      perror("Failed write to file");
      return -1;
    }
    total_written += chunk_written;
  }

  return total_written;
}

void close_files(int infd, int outfd) {
  close(outfd);
  close(infd);
}
