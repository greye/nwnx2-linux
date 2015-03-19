#ifndef NWNX_LUA_NATIVE_H
#define NWNX_LUA_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CGameObject_s CGameObject;

const char *
get_script(CGameObject *object, int event);

int
set_script(CGameObject *object, int event, const char *script);

#ifdef __cplusplus
}
#endif

#endif
