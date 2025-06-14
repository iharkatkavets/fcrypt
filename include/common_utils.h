/* fe.h */

#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include "opts_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

ssize_t write_bytes(int outfd, const uint8_t *buf, ssize_t to_write);
int write_le16(int outfd, uint16_t value);
int read_le16(int infd, uint16_t *value);
int create_input_fd(options opts, int *infd);
int check_output_file_absent(options opts);
int create_output_fd(options opts, int *outfd);
int setup_enc_key(uint8_t **key_hash32, options opts);
int setup_dec_key(uint8_t **key_hash32, options opts);
void create_password_hash(uint8_t **key_hash32, uint8_t *key, size_t keysize);
int resolve_hint(uint8_t **hint_out, size_t *hint_len_out, options opts);
ssize_t read_bytes(int infd, uint8_t *buf, ssize_t count);

#endif
