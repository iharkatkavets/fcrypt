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


ssize_t write_to_file(int outfd, uint8_t *buf, ssize_t to_write) {
  ssize_t chunk_written, total_written = 0;

  while (total_written < to_write) {
    chunk_written = write(outfd, buf + total_written, to_write - total_written);
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


int create_input_fd(options opts, int *infd) {
  if ((*infd = open(opts.input_file, O_RDONLY)) < 0) {
    fprintf(stderr, "\nCan't open '%s' file for reading", opts.input_file);
    return EXIT_FAILURE;
  } else {
    return EXIT_SUCCESS;
  }
}


int verify_output_file_not_exists(options opts) {
  if (opts.output_file && file_exist(opts.output_file)) {
    fprintf(stderr, "\nOutput file '%s' exists.", opts.output_file);
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
    fprintf(stderr, "\nCan't open '%s' file for writing", opts.output_file);
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
      fprintf(stderr, "\nAbort");
      return EXIT_FAILURE;
    }
    uint8_t *key2 = malloc(256);
    size_t keysize2 = read_input_safe("\nVerify password: ", key2, 256);
    if (!keysize2) {
      free(key1); free(key2);
      fprintf(stderr, "\nAbort");
      return EXIT_FAILURE;
    }
    if (keysize1 != keysize2 || memcmp(key1, key2, keysize1)) {
      free(key1); free(key2);
      fprintf(stderr, "\nPasswords are not the same. Abort");
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
      fprintf(stderr, "\nAbort");
      return EXIT_FAILURE;
    }
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
  vlog("\nSHA256(key): %s", key_hash_str);
}
