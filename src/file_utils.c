/* file_utils.c */

#include "file_utils.h"
#include <unistd.h>

int file_exist(char *path) {
  return (access(path, F_OK) == 0);
}
