/* decryptor.c */

#include "decryptor.h"
#include "common_utils.h"
#include "io_utils.h"
#include "xchacha20.h"
#include "sha256.h"
#include "convert_utils.h"
#include "core_utils.h"
#include "verbose.h"
#include "opts_utils.h"

int decryptor(options opts) {
  int infd, outfd;
  XChaCha_ctx ctx;
  uint8_t *key;
  uint8_t *key_hash32;
  size_t keysize = 0;
  uint16_t padsize = 0;
  char *key_hash_str;
  uint8_t nonce24[24];
  char *nonce24_str;
  uint8_t counter[8] = {0x1};
  uint8_t enc_buf[4096];
  uint8_t dec_buf[4096];
  ssize_t chunk = 0;
  ssize_t read_size = 0;

  infd = open(opts.input_file, O_RDONLY);
  if (infd < 1) {
    perror("Can't open for reading\n");
    return -1;
  }

  outfd = open(opts.output_file, O_WRONLY|O_CREAT|O_TRUNC, 00600);
  if (outfd < 1) {
    close(infd);
    perror("Can't open for writing\n");
    return -1;
  }

  key = read_input_safe("Password: ", &keysize);
  if (keysize == 0 || !key) {
    fprintf(stderr, "Stop\n");
    close_files(infd, outfd);
    return -1;
  }

  key_hash32 = sha256_data(key, keysize);
  key_hash_str = uint8_to_hex(key_hash32, 32);
  vlog("\nsha256(key): %s\n", key_hash_str);

  if (read(infd, nonce24, 24) != 24) {
    fprintf(stderr, "Cant' read 24 bytes for nonce. Stop\n");
    close_files(infd, outfd);
    return -1;
  }

  nonce24_str = uint8_to_hex(nonce24, 24);
  vlog("nonce24: %s\n", nonce24_str);

  xchacha_keysetup(&ctx, key_hash32, nonce24);
  xchacha_set_counter(&ctx, counter);

  if (read(infd, enc_buf, 2) != 2) {
    perror("Cant' read 2 bytes for padsize. Stop\n");
    close_files(infd, outfd);
    return -1;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, (uint8_t*)&padsize, 2);
  vlog("Padsize: %u", padsize);

  while (padsize > 0) {
    chunk = MIN(padsize, sizeof(enc_buf));
    read_size = read(infd, enc_buf, chunk);
    if (read_size < 0) {
      perror("Can't read file\n");
      close_files(infd, outfd);
      return -1;
    }
    if (read_size != chunk) {
      fprintf(stderr, "Wrong file or password.");
      close_files(infd, outfd);
      return -1;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, chunk);
    padsize -= chunk;
  }

  read_size = read(infd, enc_buf, 32);
  if (read_size < 0) {
    perror("Can't read file\n");
    close_files(infd, outfd);
    return -1;
  }
  if (read_size != 32) {
    fprintf(stderr, "Wrong file or password.");
    close_files(infd, outfd);
    return -1;
  }

  xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, 32);
  if (memcmp(dec_buf, key_hash32, 32) != 0) {
    close_files(infd, outfd);
    fprintf(stderr, "Wrong password.");
    return -1;
  }

  while(true) {
    read_size = read(infd, enc_buf, sizeof(enc_buf));
    if (read_size<0) {
      perror("Failed to read file\n");
      close_files(infd, outfd);
      return -1;
    }
    if (!read_size) {
      break;
    }
    xchacha_decrypt_bytes(&ctx, enc_buf, dec_buf, read_size);
    if ((write_to_file(outfd, dec_buf, read_size)) != read_size) {
      fprintf(stderr, "Failed to write to %s:%d\n", opts.output_file, __LINE__);
      close_files(infd, outfd);
      return -1;
    }
  }

  free(nonce24_str);
  free(key_hash_str); free(key_hash32);
  free(key);
  close_files(infd, outfd);

  return 0;
}
