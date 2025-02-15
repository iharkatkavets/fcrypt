/* main.c */

#include "io_utils.h"

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
  print_usage(argv[0]);
  return 0;
}
