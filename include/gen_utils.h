/* gen_utils.h */

#ifndef GEN_UTILS_H
#define GEN_UTILS_H

#include <stddef.h>
#include <stdint.h>

size_t gen_secure_bytes(uint8_t *buf, size_t len);
size_t gen_uint16(uint16_t *padsize);

#endif
