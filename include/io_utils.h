/* io_utils.h */

#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

size_t read_input_safe(char *prompt, uint8_t *out_buf, size_t out_buf_size);

#endif
