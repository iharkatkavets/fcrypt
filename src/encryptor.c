/* encryptor.c */

#include "encryptor.h"
#include "common_utils.h"
#include "xchacha20.h"
#include "convert_utils.h"
#include "core_utils.h"
#include "verbose.h"
#include "opts_utils.h"
#include "gen_utils.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


size_t gen_nonce(uint8_t *buf, size_t size) {
  char nonce24_str[size*2+1];
  size_t result; 

  if (!(result = gen_secure_bytes(buf, size))) {
    uint8_to_hex(nonce24_str, buf, size);
    vlog("\nNonce[24]: %s", nonce24_str);
  }
  return result;
}


size_t gen_pad_size(uint16_t *padsize, options opts) {
  if (opts.padsize != -1) {
    *padsize = (uint16_t)opts.padsize;
    return EXIT_SUCCESS;
  }

  if(gen_uint16(padsize)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

 
int perform_encryption(options opts, int infd, int outfd, uint8_t *key_hash32) {
  XChaCha_ctx ctx;
  uint8_t counter[8] = {0x1};
  uint8_t enc_buf[4096];
  ssize_t chunk = 0;
  uint8_t dec_buf[4096];
  ssize_t read_size = 0;
  uint8_t pad_buf[4096];
  uint8_t nonce24[24];
  uint16_t padsize = -1;

  if (gen_nonce(nonce24, 24)) {
    fprintf(stderr, "\nCan't generate IV.");
    return EXIT_FAILURE;
  }

  if ((write_to_file(outfd, nonce24, 24)) != 24) {
    fprintf(stderr, "\nFail to write to %s:%d. Abort", opts.output_file, __LINE__);
    return EXIT_FAILURE;
  }

  if (gen_pad_size(&padsize, opts)) {
    fprintf(stderr, "\nFail to generate pad size.");
    return EXIT_FAILURE;
  }
  vlog("\nPadsize: %u", padsize);

  xchacha_keysetup(&ctx, key_hash32, nonce24);
  xchacha_set_counter(&ctx, counter);

  xchacha_encrypt_bytes(&ctx, (uint8_t*)&(padsize), enc_buf, 2);
  if ((write_to_file(outfd, enc_buf, 2)) != 2) {
    fprintf(stderr, "\nFailed to write to %s:%d.", opts.output_file, __LINE__);
    return EXIT_FAILURE;
  }

  while (padsize > 0) {
    chunk = MIN((size_t)padsize, sizeof(enc_buf));
    if (gen_secure_bytes(pad_buf, chunk)) {
      fprintf(stderr, "\nFailed to generate secure bytes %s:%d.", opts.output_file, __LINE__);
      return EXIT_FAILURE;
    }
    xchacha_encrypt_bytes(&ctx, pad_buf, enc_buf, chunk);
    if ((write_to_file(outfd, enc_buf, chunk)) != chunk) {
      fprintf(stderr, "\nFailed to write to %s:%d", opts.output_file, __LINE__);
      return EXIT_FAILURE;
    }
    padsize -= chunk;
  }

  xchacha_encrypt_bytes(&ctx, key_hash32, enc_buf, 32);
  if ((write_to_file(outfd, enc_buf, 32)) != 32) {
    fprintf(stderr, "\nFailed to write to %s:%d.", opts.output_file, __LINE__);
    return EXIT_FAILURE;
  }

  while(true) {
    read_size = read(infd, dec_buf, sizeof(dec_buf));
    if (read_size < 0) {
      fprintf(stderr, "\nFailed to read file %s.", opts.input_file);
      return EXIT_FAILURE;
    }
    if (!read_size) {
      break;
    }
    xchacha_encrypt_bytes(&ctx, dec_buf, enc_buf, read_size);
    if ((write_to_file(outfd, enc_buf, read_size)) != read_size) {
      fprintf(stderr, "\nFailed to write to %s:%d.", opts.output_file, __LINE__);
      return EXIT_FAILURE;
    }
  }
  return 0;
}


int encryptor(options opts) {
  int infd, outfd;
  uint8_t *key_hash32;

  if (opts.verbose) {
    verbose = 1;
  }

  if (create_input_fd(opts, &infd)) {
    return EXIT_FAILURE;
  }

  if (verify_output_file_not_exists(opts)) {
    close(infd);
    return EXIT_FAILURE;
  }

  if (setup_enc_key(&key_hash32, opts)) {
    close(infd);
    return EXIT_FAILURE;
  }

  if (create_output_fd(opts, &outfd)) {
    free(key_hash32);
    close(infd);
    return EXIT_FAILURE;
  }

  if (perform_encryption(opts, infd, outfd, key_hash32)) {
    free(key_hash32);
    close(infd); close(outfd);
    return EXIT_FAILURE;
  }

  free(key_hash32);
  close(infd); close(outfd);

  return EXIT_SUCCESS;
}
