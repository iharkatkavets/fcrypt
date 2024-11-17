/* io_utils.h */

#ifndef IO_UTILS_H
#define IO_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>
#include <strings.h>
#include <termios.h>

uint8_t *read_input_safe(char*, size_t*);

#endif
