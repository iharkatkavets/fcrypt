/* encryptor.c */

#include "encryptor.h"
#include "common_utils.h"
#include "file_utils.h"
#include "io_utils.h"
#include "xchacha20.h"
#include "sha256.h"
#include "convert_utils.h"
#include "core_utils.h"
#include "verbose.h"
#include <getopt.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "opts_utils.h"

int encryptor(options opts) {
  int infd, outfd;
  XChaCha_ctx ctx;
  uint8_t key[256];
  uint8_t *key_hash32;
  size_t keysize = 0;
  int32_t padsize = -1;
  char *key_hash_str;
  uint8_t *nonce24;
  char *nonce24_str;
  uint8_t counter[8] = {0x1};
  uint8_t enc_buf[4096];
  ssize_t chunk = 0;
  uint8_t dec_buf[4096];
  uint8_t *pad_buf;
  ssize_t read_size = 0;

  infd = open(opts.input_file, O_RDONLY);
  if (infd < 1) {
    fprintf(stderr, "Can't open %s for reading\n", opts.input_file);
    return EXIT_FAILURE;
  }

  if (opts.output_file) {
    if (file_exist(opts.output_file)) {
      fprintf(stderr, "Output file exists\n");
      return EXIT_FAILURE;
    }
    else {
      outfd = open(opts.output_file, O_WRONLY|O_CREAT|O_TRUNC, 00600);
      if (outfd < 0) {
        close(infd);
        fprintf(stderr, "Can't open %s for writing\n", opts.output_file);
        return EXIT_FAILURE;
      }
    }
  }
  else {
    outfd = STDOUT_FILENO;
  }

  if (opts.key) {
    keysize = strlen(opts.key);
    memcpy(key, opts.key, keysize);
  }
  else {
    keysize = read_input_safe("Password: ", key, 256);
    if (!keysize) {
      fprintf(stderr, "Stop\n");
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
  }

  key_hash32 = sha256_data(key, keysize);
  key_hash_str = uint8_to_hex(key_hash32, 32);
  vlog("\nsha256(key): %s\n", key_hash_str);

  if (!(nonce24 = gen_nonce(24))) {
    fprintf(stderr, "Stop\n");
    close_files(infd, outfd);
    return EXIT_FAILURE;
  }

  nonce24_str = uint8_to_hex(nonce24, 24);
  vlog("nonce24: %s\n", nonce24_str);

  if ((write_to_file(outfd, nonce24, 24)) != 24) {
    fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
    close_files(infd, outfd);
    return EXIT_FAILURE;
  }

  xchacha_keysetup(&ctx, key_hash32, nonce24);
  xchacha_set_counter(&ctx, counter);

  if (padsize == -1) {
    padsize = gen_uint16();
  }

  vlog("Padsize: %u", padsize);

  xchacha_encrypt_bytes(&ctx, (uint8_t*)&padsize, enc_buf, 2);

  if ((write_to_file(outfd, enc_buf, 2)) != 2) {
    fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
    close_files(infd, outfd);
    return EXIT_FAILURE;
  }

  while (padsize > 0) {
    chunk = MIN((size_t)padsize, sizeof(enc_buf));
    if (!(pad_buf = gen_secure_bytes(chunk))) {
      fprintf(stderr, "Failed to generate secure bytes %s:%d\n", opts.output_file, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
    xchacha_encrypt_bytes(&ctx, pad_buf, enc_buf, chunk);
    if ((write_to_file(outfd, enc_buf, chunk)) != chunk) {
      fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
    free(pad_buf);
    padsize -= chunk;
  }

  xchacha_encrypt_bytes(&ctx, key_hash32, enc_buf, 32);
  if ((write_to_file(outfd, enc_buf, 32)) != 32) {
    fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
    close_files(infd, outfd);
    return EXIT_FAILURE;
  }

  while(true) {
    read_size = read(infd, dec_buf, sizeof(dec_buf));
    if (read_size<0) {
      perror("Failed to read file\n");
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
    if (!read_size) {
      break;
    }
    xchacha_encrypt_bytes(&ctx, dec_buf, enc_buf, read_size);
    if ((write_to_file(outfd, enc_buf, read_size)) != read_size) {
      fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
  }

  free(nonce24_str); free(nonce24);
  free(key_hash_str); free(key_hash32);
  close_files(infd, outfd);

  return EXIT_SUCCESS;
}
