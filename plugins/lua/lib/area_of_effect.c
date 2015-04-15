#include "native.h"
#include "engine.h"

#include <math.h>

int
set_aoe_shape(CGameObject *object, int shape, float width, float length) {
    CNWSAreaOfEffectObject * const aoe =
        object->vtable->AsNWSAreaOfEffectObject(object);
    if (!aoe || shape) {
        return -1;
    }
    if (!shape) {
        aoe->shape = shape;
        aoe->radius = width;
    }
    // TODO implement rectangles wrt orientation
    return 0;
}
