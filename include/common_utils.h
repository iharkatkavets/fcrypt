/* fe.h */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "opts_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

ssize_t write_to_file(int outfd, uint8_t *buf, ssize_t to_write);
int create_input_fd(options opts, int *infd);
int verify_output_file_not_exists(options opts);
int create_output_fd(options opts, int *outfd);
int setup_enc_key(uint8_t **key_hash32, options opts);
int setup_dec_key(uint8_t **key_hash32, options opts);
void create_password_hash(uint8_t **key_hash32, uint8_t *key, size_t keysize);

#endif
