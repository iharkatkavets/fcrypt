/* print_utils.c */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

char* uint8_to_hex(uint8_t *in_buf, size_t buf_size) {
  char *out_buf;

  if (!(out_buf = (char *)malloc(buf_size*2+1))) {
    fprintf(stderr, "Memory allocation failed\n");
    return NULL;
  }

  for (size_t i=0; i<buf_size; i++) {
    sprintf(out_buf+(i*2), "%02x", in_buf[i]);
  }
  out_buf[buf_size*2] = '\0';

  return out_buf;
}
