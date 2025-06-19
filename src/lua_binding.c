#include <lua.h>
#include <lauxlib.h>
#include "version.h"

int lua_get_tool_version(lua_State *L) {
    lua_pushstring(L, TOOL_VERSION);
    return 1;
}

int lua_get_format_version(lua_State *L) {
    lua_newtable(L);
    lua_pushinteger(L, FORMAT_VERSION[0]);
    lua_rawseti(L, -2, 1);
    lua_pushinteger(L, FORMAT_VERSION[1]);
    lua_rawseti(L, -2, 2);
    return 1;
}


int luaopen_mytool(lua_State *L) {
    luaL_Reg funcs[] = {
        {"get_tool_version", lua_get_tool_version},
        {"get_format_version", lua_get_format_version},
        {NULL, NULL}
    };

    luaL_newlib(L, funcs);
    return 1;
}
