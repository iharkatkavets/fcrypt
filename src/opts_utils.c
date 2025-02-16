/* opts_utils.c */

#include "opts_utils.h"
#include "version.h"
#include <getopt.h>
#include <stdio.h>

void print_usage(const char *program_name) {
  printf("Usage: %s [-e <FILE> | --encrypt <FILE>] [-d <FILE> | --decrypt <FILE>] [-p <value> | --padsize <value>] [-o | --output <FILE>] [-h | --help]\n", program_name);
  printf("Options:\n");
  printf("  -d, --decrypt <FILE>    Run Decrypt operation.\n");
  printf("  -e, --encrypt <FILE>    Run Encrypt operation.\n");
  printf("  -p, --padsize <value>   Optional. Size of random bytes for padding. Generated randomly in range 0-65555 if not provided.\n");
  printf("  -o, --output <FILE>     Output file.\n");
  printf("  -k, --key <PASSWORD>    The password for encrypt.\n");
  printf("  -v, --verbose           Enable verbose output.\n");
  printf("  -V                      Display the version number and exit.\n");
  printf("  -h, --help              Show this help message and exit.\n");
  printf("Examples:\n");
  printf("  %s -e origin.file -o encrypted.file\n", program_name);
  printf("  %s -e origin.file -o encrypted.file -k 'strong password'\n", program_name);
  printf("  %s -e origin.file > encrypted.file\n", program_name);
  printf("  %s -d encrypted.file\n", program_name);
  printf("  %s -d encrypted.file -k 'strong password'\n", program_name);
  printf("\n");
  printf("Description:\n");
  printf("  A command-line tool for encrypting files using the XChaCha20 algorithm.\n");
  printf("  Provide an input file. Optional parameters\n");
  printf("  include padding size and verbose mode for detailed logs.\n");
}

void print_version(const char *program_name) {
  printf("%s %s\n", program_name, TOOL_VERSION);
}

int parse_arguments(options *opts, int argc, char **argv) {
  int option = -1;
  int option_index = 0;

  struct option long_options[] = {
    {"padsize", required_argument, 0, 'p'}, 
    {"encrypt", required_argument, 0, 'e'},
    {"key",     required_argument, 0, 'k'},
    {"decrypt", required_argument, 0, 'd'},
    {"output",  required_argument, 0, 'o'}, 
    {"verbose", no_argument,       0, 'v'},
    {"help",    no_argument,       0, 'h'},
    {0,         0,                 0,  0 }
  };

  while ((option = getopt_long(argc, argv, "d:e:p:o:k:hvV", long_options, &option_index)) != -1) {
    switch (option) {
      case 'p':
        (*opts).padsize = atoi(optarg);
        if ((*opts).padsize < 0 || (*opts).padsize > 65535) {
          fprintf(stderr, "Invalid pad size. Must be in range [0, 65535].\n");
          return EXIT_FAILURE;
        }
        break;
      case 'e':
        (*opts).encrypt = 1;
        (*opts).input_file = optarg;
        break;
      case 'o':
        (*opts).output_file = optarg;
        break;
      case 'd':
        (*opts).decrypt = 1;
        (*opts).input_file = optarg;
        break;
      case 'k':
        (*opts).key = optarg;
        break;
      case 'v':
        (*opts).verbose = 1;
        break;
      case 'V':
        (*opts).show_version = 1;
        break;
      case 'h':
        (*opts).show_help = 1;
        return EXIT_SUCCESS;
      case '?':
      default:
        return EXIT_FAILURE;
    }
  }

  if ((*opts).encrypt && (*opts).decrypt) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
