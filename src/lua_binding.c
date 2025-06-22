#include <lua.h>
#include <lauxlib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "decrypt.h"

int lua_get_fcrypt_version(lua_State *L) {
    lua_pushstring(L, FCRYPT_VERSION);
    return 1;
}

static int lua_fcrypt_decrypt_buf(lua_State *L) {
    size_t in_len, key_len;
    const char *in_str = luaL_checklstring(L, 1, &in_len);
    const char *key_str = luaL_checklstring(L, 2, &key_len);
    size_t out_buf_size = 0;
    uint8_t *out_buf = NULL;

    int ret = fcrypt_decrypt_buf(
        (const uint8_t *)in_str, in_len,
        (uint8_t *)key_str, key_len,
        &out_buf, &out_buf_size
    );

    if (ret != EXIT_SUCCESS) {
        free(out_buf);
        return luaL_error(L, "decryption failed");
    }

    lua_pushlstring(L, (const char *)out_buf, out_buf_size);
    free(out_buf);
    return 1;
}

int luaopen_fcrypt_lua_mod(lua_State *L) {
  luaL_Reg funcs[] = {
    {"get_fcrypt_version", lua_get_fcrypt_version},
    {"decrypt_buf", lua_fcrypt_decrypt_buf},
    {NULL, NULL}
  };

  luaL_newlib(L, funcs);
  return 1;
}
