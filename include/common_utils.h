/* fe.h */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <stdio.h>
#include <stdlib.h>

int write_to_file(int outfd, uint8_t *buf, ssize_t to_write);
void close_files(int infd, int outfd);

#endif
