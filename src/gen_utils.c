/* gen_utils.c */

#include "gen_utils.h"
#include "core_utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/random.h>
#include <termios.h>
#include <stdbool.h>

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
    size_t chunk = MIN(remain, (size_t)256);
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
