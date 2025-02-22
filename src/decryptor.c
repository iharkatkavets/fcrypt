/* decryptor.c */

#include "decryptor.h"
#include "common_utils.h"
#include "xchacha20.h"
#include "convert_utils.h"
#include "core_utils.h"
#include "verbose.h"
#include "opts_utils.h"

#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

int perform_decryption(options opts, int infd, int outfd, uint8_t *key_hash32) {
  XChaCha_ctx ctx;
  uint16_t padsize = 0;
  uint8_t nonce24[24];
  char nonce24_str[24*2+1];
  uint8_t counter[8] = {0x1};
  uint8_t enc_buf[4096];
  uint8_t dec_buf[4096];
  ssize_t chunk = 0;
  ssize_t read_size = 0;

  if (read(infd, nonce24, 24) != 24) {
    fprintf(stderr, "\nWrong input file. Can't read 24 bytes of nonce.");
    return EXIT_FAILURE;
  }

  uint8_to_hex(nonce24_str, nonce24, 24);
  vlog("\nNonce[24]: %s", nonce24_str);

  xchacha_keysetup(&ctx, key_hash32, nonce24);
  xchacha_set_counter(&ctx, counter);

  if (read(infd, enc_buf, 2) != 2) {
    fprintf(stderr, "\nCan't read 2 bytes of padsize.");
    return EXIT_FAILURE;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, (uint8_t*)&padsize, 2);
  vlog("\nPadsize: %u", padsize);

  while (padsize > 0) {
    chunk = MIN(padsize, sizeof(enc_buf));
    read_size = read(infd, enc_buf, chunk);
    if (read_size < 0) {
      fprintf(stderr, "\nCan't read file.");
      return EXIT_FAILURE;
    }
    if (read_size != chunk) {
      fprintf(stderr, "\nWrong file or password.");
      return EXIT_FAILURE;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, chunk);
    padsize -= chunk;
  }

  read_size = read(infd, enc_buf, 32);
  if (read_size < 0) {
    fprintf(stderr, "\nCan't read file.");
    return EXIT_FAILURE;
  }
  if (read_size != 32) {
    fprintf(stderr, "\nWrong file or password.");
    return EXIT_FAILURE;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, 32);
  if (memcmp(dec_buf, key_hash32, 32) != 0) {
    fprintf(stderr, "\nWrong password.");
    return EXIT_FAILURE;
  }

  if (outfd == STDOUT_FILENO) {
    printf("\n");
  }

  while(true) {
    read_size = read(infd, enc_buf, sizeof(enc_buf));
    if (read_size<0) {
      fprintf(stderr, "\nCan't read file %s:%d.", opts.input_file, __LINE__);
      return EXIT_FAILURE;
    }
    if (!read_size) {
      break;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, read_size);
    if ((write_to_file(outfd, dec_buf, read_size)) != read_size) {
      fprintf(stderr, "\nFail write to %s:%d.", opts.output_file, __LINE__);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}


int decryptor(options opts) {
  int infd, outfd = STDOUT_FILENO;
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

  if (setup_dec_key(&key_hash32, opts)) {
    close(infd);
    return EXIT_FAILURE;
  }

  if (create_output_fd(opts, &outfd)) {
    free(key_hash32);
    close(infd);
    return EXIT_FAILURE;
  }

  if (perform_decryption(opts, infd, outfd, key_hash32)) {
    free(key_hash32);
    close(infd); close(outfd);
    return EXIT_FAILURE;
  }

  free(key_hash32);
  close(infd); close(outfd);

  return 0;
}
