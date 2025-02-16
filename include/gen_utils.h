/* gen_utils.h */

#ifndef GEN_UTILS_H
#define GEN_UTILS_H

#include <stddef.h>
#include <stdint.h>

uint8_t *gen_secure_bytes(size_t len);
uint16_t gen_uint16();
uint8_t *gen_nonce(size_t);

#endif
