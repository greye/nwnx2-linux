#ifndef NWNX_LUA_NATIVE_H
#define NWNX_LUA_NATIVE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define offset_ptr(ptr, off) ((void *) (((char *) (ptr)) + (off)))

typedef struct CGameObject_s CGameObject;
typedef void *(*CastFunctor)(CGameObject *);

const char *
get_script(CGameObject *object, int event);

int
set_script(CGameObject *object, int event, const char *script);

int
set_aoe_shape(CGameObject *object, int shape, float width, float length);

#ifdef __cplusplus
}
#endif

#endif
