#include "lib/native.h"

#include <NWNXLib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

static void *
llua_checkptr(lua_State *L, int index, const char *name) {
    void *ptr = lua_touserdata(L, index);
    if (ptr == NULL) {
        luaL_typerror(L, index, name);
    }
    return ptr;
}

static int
lua_native_getoid(lua_State *L) {
    CGameObject *ptr = llua_checkptr(L, 1, "CGameObject");
    lua_pushinteger(L, ptr->id);

    return 1;
}

static int
lua_native_typeof(lua_State *L) {
    CGameObject *ptr = llua_checkptr(L, 1, "CGameObject");
    lua_pushinteger(L, ptr->type);

    return 1;
}

static int
lua_native_setscript(lua_State *L) {
    CGameObject *ptr = llua_checkptr(L, 1, "CGameObject");
    int event = luaL_checkint(L, 2);
    const char *name = luaL_checkstring(L, 3);

    if (set_script(ptr, event, name) != 0) {
        /* TODO report error */
    }
    return 0;
}

static int
lua_native_getscript(lua_State *L) {
    CGameObject *ptr = llua_checkptr(L, 1, "CGameObject");
    int event = luaL_checkint(L, 2);

    const char *name = get_script(ptr, event);
    if (name != NULL) {
        lua_pushstring(L, name);
    } else {
        /* TODO report error */
    }
    return 1;
}

static const luaL_Reg nativelib[] = {
    { "getoid", lua_native_getoid },
    { "getscript", lua_native_getscript },
    { "setscript", lua_native_setscript },
    { "typeof", lua_native_typeof },
    { NULL, NULL }
};

#define ARRAY_LEN(x) (sizeof(x) / sizeof(x[0]))

int
luaopen_nwn_native(lua_State *L) {
	lua_createtable(L, 0, ARRAY_LEN(nativelib) - 1);
	luaL_register(L, NULL, nativelib);
	return 1;
}
