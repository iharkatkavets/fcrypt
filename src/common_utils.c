/* main.c */

#include "common_utils.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/random.h>
#include <errno.h>
#include <unistd.h>
#include "core_utils.h"

uint8_t *gen_secure_bytes(const size_t size) {
  uint8_t *buf, *p;
  size_t remain = size;

  if (size == 0) {
    return NULL;
  }

  if (!(buf = (uint8_t *)malloc(size))) {
    return NULL;
  }

  p = buf;

  while (remain) {
    size_t chunk = MIN(remain, 256);
    if (getentropy(p, chunk)<0) {
      fprintf(stderr, "Failed to generate random data");
      free(buf);
      return NULL;
    }
    p += chunk;
    remain -= chunk;
  }

  return buf;
}

uint16_t gen_uint16() {
  uint8_t *padsize_buf;
  uint16_t padsize = 0;

  if (!(padsize_buf = gen_secure_bytes(2))) {
    fprintf(stderr, "Failed to generate padsize");
    return 0;
  }

  padsize = ((uint16_t)padsize_buf[0] << 8) | padsize_buf[1];
  free(padsize_buf);
  return padsize;
}

uint8_t *gen_nonce(size_t size) {
  uint8_t *buf;
  buf = gen_secure_bytes(size);
  return buf;
}

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
