/* io_utils.h */

#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

size_t fcrypt_read_password(const char *prompt, uint8_t *out_buf, size_t out_buf_size);
size_t fcrypt_read_str(const char *prompt, uint8_t *out_buf, size_t out_buf_size);

#endif
