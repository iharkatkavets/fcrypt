/* main.c */

#include "common_utils.h"

#include "file_utils.h"
#include "io_utils.h"
#include "sha256.h"
#include "convert_utils.h"
#include "verbose.h"

#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


ssize_t write_bytes(int outfd, const uint8_t *buf, ssize_t count) {
  ssize_t chunk_written, total_written = 0;

  while (total_written < count) {
    chunk_written = write(outfd, buf + total_written, count - total_written);
    if (chunk_written < 0) {
      if (errno == EINTR) {
        continue; 
      }
      return -1;
    }
    if (chunk_written == 0) {
      return -1;
    }
    total_written += chunk_written;
  }

  return total_written;
}

int write_le16(int outfd, uint16_t value) {
  uint8_t buf[2];
  buf[0] = value & 0xFF;        // low byte
  buf[1] = (value >> 8) & 0xFF; // high byte

  return write(outfd, buf, 2) == 2 ? 2 : -1;
}

int read_le16(int infd, uint16_t *value) {
  uint8_t buf[2];
  ssize_t read_len = read(infd, buf, 2);
  if (read_len != 2) 
    return -1;
  *value = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
  return 0;
}

int create_input_fd(options opts, int *infd) {
  if ((*infd = open(opts.input_file, O_RDONLY)) < 0) {
    fprintf(stderr, "Can't open '%s' file for reading.\n", opts.input_file);
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}

int check_output_file_absent(options opts) {
  if (opts.output_file && file_exist(opts.output_file)) {
    fprintf(stderr, "Output file '%s' exists.\n", opts.output_file);
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}


int create_output_fd(options opts, int *outfd) {
  if (!opts.output_file) {
    *outfd = STDOUT_FILENO;
    return 0;
  }

  if ((*outfd = open(opts.output_file, O_WRONLY|O_CREAT|O_TRUNC, 00600)) == -1) {
    fprintf(stderr, "Can't open '%s' file for writing.\n", opts.output_file);
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}


int setup_enc_key(uint8_t **key_hash32, options opts) {
  uint8_t *key1;
  size_t keysize1;

  if (opts.password) {
    keysize1 = strlen(opts.password);
    key1 = malloc(keysize1);
    memcpy(key1, opts.password, keysize1);
  } else {
    key1 = malloc(256);
    keysize1 = read_input_safe("Enter password: ", key1, 256);
    if (!keysize1) {
      free(key1);
      fprintf(stderr, "\nAbort\n");
      return EXIT_FAILURE;
    } 
    uint8_t *key2 = malloc(256);
    size_t keysize2 = read_input_safe("\nVerify password: ", key2, 256);
    if (!keysize2) {
      free(key1); free(key2);
      fprintf(stderr, "\nAbort\n");
      return EXIT_FAILURE;
    } 
    if (keysize1 != keysize2 || memcmp(key1, key2, keysize1)) {
      free(key1); free(key2);
      fprintf(stderr, "\nError: Passwords do not match.");
      return EXIT_FAILURE;
    }
    free(key2);
  }

  create_password_hash(key_hash32, key1, keysize1);
  memset(key1, 0, keysize1);
  free(key1);

  return EXIT_SUCCESS;
}


int setup_dec_key(uint8_t **key_hash32, options opts) {
  uint8_t *key1;
  size_t keysize1;

  if (opts.password) {
    keysize1 = strlen(opts.password);
    key1 = malloc(keysize1);
    memcpy(key1, opts.password, keysize1);
  } else {
    key1 = malloc(256);
    keysize1 = read_input_safe("Enter password: ", key1, 256);
    if (!keysize1) {
      free(key1);
      fprintf(stderr, "\nAbort\n");
      return EXIT_FAILURE;
    }
    fprintf(stderr, "\n");
  }

  create_password_hash(key_hash32, key1, keysize1);
  memset(key1, 0, keysize1);
  free(key1);

  return EXIT_SUCCESS;
}


void create_password_hash(uint8_t **key_hash32, uint8_t *key, size_t keysize) {
  char key_hash_str[32*2+1];
  *key_hash32 = sha256_data(key, keysize);
  uint8_to_hex(key_hash_str, *key_hash32, 32);
  vlog("SHA256(key): %s\n", key_hash_str);
}


int resolve_hint(uint8_t **hint_out, size_t *hint_len_out, options opts) {
  if (opts.hint) {
    *hint_len_out = strlen(opts.hint);
    *hint_out = malloc(*hint_len_out);
    if (!*hint_out) return EXIT_FAILURE;
    memcpy(*hint_out, opts.hint, *hint_len_out);
  } else {
    size_t hintsize;
    uint8_t *hint = malloc(256);
    if (!hint) return EXIT_FAILURE;

    hintsize = read_input("\nEnter password hint: ", hint, 256);
    if (!hintsize) {
      *hint_out = NULL;
      *hint_len_out = 0;
      free(hint);
      return EXIT_SUCCESS;
    }

    *hint_len_out = hintsize;
    *hint_out = malloc(hintsize);
    if (!*hint_out) {
      free(hint);
      return EXIT_FAILURE;
    }

    memcpy(*hint_out, hint, hintsize);
    free(hint);
  }

  return EXIT_SUCCESS;
}

ssize_t read_bytes(int infd, uint8_t *buf, ssize_t count) {
  ssize_t read_len = read(infd, buf, count);
  if (read_len != count) 
    return -1;
  return count;
}
