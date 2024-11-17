/* fe.h */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stddef.h>
#include <string.h>
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
#include "xchacha20.h"

uint8_t *gen_secure_bytes(size_t len);
uint16_t gen_uint16();
uint8_t *gen_nonce(size_t);
int write_to_file(int outfd, uint8_t *buf, ssize_t to_write);
void close_files(int infd, int outfd);

#endif
