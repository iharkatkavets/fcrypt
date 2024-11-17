/* encryptor.c */

#include "common_utils.h"
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
#include <unistd.h>

void print_usage(const char *program_name) {
  printf("Usage: %s [-p <value> | --padsize <value>] [-h | --help] <input_file> <output_file>\n", program_name);
  printf("Options:\n");
  printf("  -p, --padsize <value>   Optional. Size of random bytes for padding. Generated randomly in range 0-65555 if not provided.\n");
  printf("  -v, --verbose           Enable verbose output.\n");
  printf("  -h                      Show this help message and exit.\n");
  printf("Examples:\n");
    printf("  %s -p 16 input.file encrypted.file\n", program_name);
  printf("\n");
    printf("Description:\n");
    printf("  A command-line tool for encrypting files using the XChaCha20 algorithm.\n");
    printf("  Provide an input file. Optional parameters\n");
    printf("  include padding size and verbose mode for detailed logs.\n");
}

int main(int argc, char *argv[]) {
  char *infile, *outfile;
  int infd, outfd;
  XChaCha_ctx ctx;
  uint8_t *key;
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
  int option_index = 0;
  int option;

  struct option long_options[] = {
    {"padsize", required_argument, 0, 'p'}, 
    {"verbose", no_argument, 0, 'v'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
  };

  while ((option = getopt_long(argc, argv, "p:vh", long_options, &option_index)) != -1) {
    switch (option) {
      case 'p':
        padsize = atoi(optarg);
        if (padsize < 0 || padsize > 65535) {
          fprintf(stderr, "Invalid pad size. Must be in range [0, 65535].\n");
          return EXIT_FAILURE;
        }
        break;
      case 'v':
        verbose = 1;
        break;
      case 'h':
        print_usage(argv[0]);
        return EXIT_SUCCESS;
      case '?':
      default:
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }
  }

  if (optind+2 > argc) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  infile = argv[optind];
  outfile = argv[optind+1];

  infd = open(infile, O_RDONLY);
  if (infd < 1) {
    fprintf(stderr, "Can't open %s for reading\n", infile);
    return EXIT_FAILURE;
  }

  outfd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, 00600);
  if (outfd < 1) {
    close(infd);
    fprintf(stderr, "Can't open %s for writing\n", outfile);
    return EXIT_FAILURE;
  }

  key = read_input_safe("Password: ", &keysize);
  if (!keysize || !key) {
    fprintf(stderr, "Stop\n");
    close_files(infd, outfd);
    return EXIT_FAILURE;
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
    fprintf(stderr, "Failed to write to %s:%d\n", outfile, __LINE__);
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
    fprintf(stderr, "Failed to write to %s:%d\n", outfile, __LINE__);
    close_files(infd, outfd);
    return EXIT_FAILURE;
  }

  while (padsize > 0) {
    chunk = MIN((size_t)padsize, sizeof(enc_buf));
    if (!(pad_buf = gen_secure_bytes(chunk))) {
      fprintf(stderr, "Failed to generate secure bytes %s:%d\n", outfile, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
    xchacha_encrypt_bytes(&ctx, pad_buf, enc_buf, chunk);
    if ((write_to_file(outfd, enc_buf, chunk)) != chunk) {
      fprintf(stderr, "Failed to write to %s:%d\n", outfile, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
    free(pad_buf);
    padsize -= chunk;
  }

  xchacha_encrypt_bytes(&ctx, key_hash32, enc_buf, 32);
  if ((write_to_file(outfd, enc_buf, 32)) != 32) {
    fprintf(stderr, "Failed to write to %s:%d\n", outfile, __LINE__);
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
      fprintf(stderr, "Failed to write to %s:%d\n", outfile, __LINE__);
      close_files(infd, outfd);
      return EXIT_FAILURE;
    }
  }

  free(nonce24_str); free(nonce24);
  free(key_hash_str); free(key_hash32);
  free(key);
  close_files(infd, outfd);

  return EXIT_SUCCESS;
}
