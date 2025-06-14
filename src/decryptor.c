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
#include <stdlib.h>
#include <termios.h>
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
    fprintf(stderr, "Wrong input file. Can't read 24 bytes of nonce.\n");
    return EXIT_FAILURE;
  }

  uint8_to_hex(nonce24_str, nonce24, 24);
  vlog("\nNonce[24]: %s", nonce24_str);

  xchacha_keysetup(&ctx, key_hash32, nonce24);
  xchacha_set_counter(&ctx, counter);

  if (read(infd, enc_buf, 2) != 2) {
    fprintf(stderr, "Can't read 2 bytes of padsize.\n");
    return EXIT_FAILURE;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, (uint8_t*)&padsize, 2);
  vlog("Padsize: %u\n", padsize);

  while (padsize > 0) {
    chunk = MIN(padsize, sizeof(enc_buf));
    read_size = read(infd, enc_buf, chunk);
    if (read_size < 0) {
      fprintf(stderr, "Can't read file.\n");
      return EXIT_FAILURE;
    }
    if (read_size != chunk) {
      fprintf(stderr, "Wrong file or password.\n");
      return EXIT_FAILURE;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, chunk);
    padsize -= chunk;
  }

  read_size = read(infd, enc_buf, 32);
  if (read_size < 0) {
    fprintf(stderr, "Can't read file.\n");
    return EXIT_FAILURE;
  }
  if (read_size != 32) {
    fprintf(stderr, "Wrong file or password.\n");
    return EXIT_FAILURE;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, 32);
  if (memcmp(dec_buf, key_hash32, 32) != 0) {
    fprintf(stderr, "Wrong password.\n");
    return EXIT_FAILURE;
  }

  while(true) {
    read_size = read(infd, enc_buf, sizeof(enc_buf));
    if (read_size<0) {
      fprintf(stderr, "Can't read file %s:%d.\n", opts.input_file, __LINE__);
      return EXIT_FAILURE;
    }
    if (!read_size) {
      break;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, read_size);
    if ((write_bytes(outfd, dec_buf, read_size)) != read_size) {
      fprintf(stderr, "Fail write to %s:%d.\n", opts.output_file, __LINE__);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int extract_hint(int infd, uint8_t **hint, uint16_t *hint_len) {
  uint16_t len = 0;
  if (read_le16(infd, &len) != 0) {
    return EXIT_FAILURE;
  }

  *hint = malloc(len);
  if (*hint == NULL) {
    return EXIT_FAILURE;
  }
  if (read_bytes(infd, *hint, len) != len) {
    free(*hint);
    return EXIT_FAILURE;
  }

  *hint_len = len;
  return EXIT_SUCCESS;
}

int extract_version(int infd, uint16_t *version) {
  if (read_le16(infd, version) != 0) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int decryptor(options opts) {
  int infd, outfd = STDOUT_FILENO;
  uint8_t *key_hash32;
  uint16_t version = 0;
  uint8_t *hint;
  uint16_t hint_len;

  if (opts.verbose) {
    verbose = 1;
  }

  if (create_input_fd(opts, &infd)) {
    return EXIT_FAILURE;
  }

  if (check_output_file_absent(opts)) {
    close(infd);
    return EXIT_FAILURE;
  }

  if (extract_version(infd, &version)) {
    close(infd);
    return EXIT_FAILURE;
  }

  if (version > 0) {
    if (extract_hint(infd, &hint, &hint_len)) {
      close(infd);
      return EXIT_FAILURE;
    }
    if (hint_len == 0) {
      fprintf(stderr, "No password hint available.\n");
    } else {
      fprintf(stderr, "Hint: %s\n", hint);
      free(hint);
    }
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

