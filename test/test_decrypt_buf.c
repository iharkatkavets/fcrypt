#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include "decrypt.h"

int main(void) {
  FILE *fp = fopen("testdata/encrypted.file", "rb");
  if (!fp) {
    perror("fopen");
    return EXIT_FAILURE;
  }

  fseek(fp, 0, SEEK_END);
  size_t enc_len = ftell(fp);
  fseek(fp, 0, SEEK_SET);

  uint8_t *encrypted = malloc(enc_len);
  fread(encrypted, 1, enc_len, fp);
  fclose(fp);

  const char *password = "12345";
  uint8_t *key_hash32 = NULL;

  uint8_t *output = NULL;
  size_t out_len = 0;

  int rc = fcrypt_decrypt_buf(
    encrypted, 
    enc_len, 
    (uint8_t *)password, 
    strlen((char *)password), 
    &output, 
    &out_len);

  if (rc != EXIT_SUCCESS) {
    fprintf(stderr, "decryption failed\n");
    return 1;
  }

  fwrite(output, 1, out_len, stdout);

  free(encrypted);
  free(output);
  free(key_hash32);

  return 0;
}
