/* opts_utils.c */

#include "opts_utils.h"
#include "version.h"
#include <getopt.h>
#include <stdio.h>

void print_usage(const char *program_name) {
  printf("Usage: %s [-e <FILE> | --encrypt <FILE>] [-d <FILE> | --decrypt <FILE>] [-p <value> | --padsize <value>] [-o | --output <FILE>] [-h | --help]\n", program_name);
  printf("Options:\n");
  printf("  -d, --decrypt <FILE>        Run Decrypt operation.\n");
  printf("  -e, --encrypt <FILE>        Run Encrypt operation.\n");
  printf("  -l, --length <value>        Optional. Size of random bytes for padding. Generated randomly in range 0-65555 if not provided.\n");
  printf("  -o, --output <FILE>         Output file.\n");
  printf("  -p --password <PASSWORD>    The password for encrypt.\n");
  printf("  -v, --verbose               Enable verbose output.\n");
  printf("  -V                          Display the version number and exit.\n");
  printf("  -h, --help                  Show this help message and exit.\n");
  printf("Examples:\n");
  printf("  %s -e origin.file -o encrypted.file\n", program_name);
  printf("  %s -e origin.file -o encrypted.file -p 'strong password'\n", program_name);
  printf("  %s -e origin.file > encrypted.file\n", program_name);
  printf("  %s -d encrypted.file\n", program_name);
  printf("  %s -d encrypted.file -p 'strong password'\n", program_name);
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
  int padsize = -1;

  struct option long_options[] = {
    {"encrypt", required_argument, 0, 'e'},
    {"length",  required_argument, 0, 'l'}, 
    {"password",required_argument, 0, 'p'},
    {"decrypt", required_argument, 0, 'd'},
    {"output",  required_argument, 0, 'o'}, 
    {"verbose", no_argument,       0, 'v'},
    {"help",    no_argument,       0, 'h'},
    {0,         0,                 0,  0 }
  };

  while ((option = getopt_long(argc, argv, "e:l:p:d:o:hvV", long_options, &option_index)) != -1) {
    switch (option) {
      case 'l':
        padsize = atoi(optarg);
        if (padsize < 0 || padsize > 65535) {
          fprintf(stderr, "Invalid pad size. Must be in range [0, 65535].\n");
          return EXIT_FAILURE;
        }
        break;
      case 'e':
        opts->encrypt = 1;
        opts->input_file = optarg;
        break;
      case 'o':
        opts->output_file = optarg;
        break;
      case 'd':
        opts->decrypt = 1;
        opts->input_file = optarg;
        break;
      case 'p':
        opts->password = optarg;
        break;
      case 'v':
        opts->verbose = 1;
        break;
      case 'V':
        opts->show_version = 1;
        break;
      case 'h':
        opts->show_help = 1;
      case '?':
      default:
        return EXIT_FAILURE;
    }
  }

  if (opts->encrypt && opts->decrypt) {
    return EXIT_FAILURE;
  }
  opts->padsize = padsize;

  return EXIT_SUCCESS;
}
