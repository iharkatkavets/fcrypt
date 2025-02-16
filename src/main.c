/* main.c */

#include "decryptor.h"
#include "encryptor.h"
#include "opts_utils.h"

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  options opts = {0};
  if (parse_arguments(&opts, argc, argv) == EXIT_FAILURE) {
    fprintf(stderr, "Error: Wrong parameters\n");
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  if (opts.show_help) {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  if (opts.show_version) {
    print_version(argv[0]);
    return EXIT_SUCCESS;
  }

  if (opts.decrypt) {
    decryptor(opts);
  }
  else if (opts.encrypt) {
    encryptor(opts);
  }
  else {
    print_usage(argv[0]);
  }

  return EXIT_SUCCESS;
}
