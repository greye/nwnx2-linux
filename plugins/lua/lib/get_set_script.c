#include "native.h"

#include <nx_devel.h>
#include <nwn_types.h>

#include <stddef.h>
#include <string.h>

typedef void (*set_script_func)(void *, int, CExoString *);
typedef CExoString *(*get_script_func)(void *, int);

static const struct {
    char type;
    char nevents;
} headers[] = {
    { 3, 18 },
    { 4, 4 },
    { 5, 13 },
    { 7, 7 },
    { 9, 16 },
    { 10, 15 },
    { 13, 5 },
    { 14, 2 }
};

#define METHODS(name, get, set) offsetof(CGameObject_VTable, AsNWS##name), ((void *) get), ((void *) set), #name
static const struct Reciever {
    const int reject[4];
    size_t offset;
    get_script_func get;
    set_script_func set;
    const char *name;
} receivers[] = {
    { { 3, -1, -1, -1 },  METHODS(Module, 0x081c1050, 0x81c106c) },
    { { -1, -1, -1, -1 }, METHODS(Area, 0x080d55cc, 0x080d55e8) },
    { { -1, -1, -1, -1 }, METHODS(Creature, 0x0813d704, 0x0813d720) },
    { { 4, 5, -1, -1 },   METHODS(Trigger, 0x081fc2cdc, 0x081f2df4) },
    { { 3, 10, 14, -1 },  METHODS(Placeable, 0x081e23c8, 0x081e23e4) },
    { { 4, 9, 13, -1 },   METHODS(Door, 0x0816afb8, 0x0816af58) },
    { { -1, -1, -1, -1 }, METHODS(Encounter, 0x081845c8, 0x081845e4) },
    { { -1, -1, -1, -1 }, METHODS(Store, 0x080861b4, 0x0806814c) }
};

#define ARRAY_LENGTH(x) (sizeof(x) / sizeof(*x))

static int
check_receiver(int index, int event) {
    const int *reject = receivers[index].reject;
    if ((reject[0] != event) &
        (reject[1] != event) &
        (reject[2] != event) &
        (reject[3] != event)) {
        return index;
    }
    return -1;
}

static int
find_receiver(int type, int event) {
    int i;
    if (event >= 0) {
        for (i = 0; i < ARRAY_LENGTH(receivers); ++i) {
            if (type == headers[i].type) {
                if (event < headers[i].nevents) {
                    return check_receiver(i, event);
                }
                return -1;
            }
        }
    }
    return -1;
}


#define CAST(obj, rcv) \
    (*(CastFunctor *) offset_ptr((obj)->vtable, (rcv)->offset))(obj)

int
set_script(CGameObject *object, int event, const char *script) {
    const int index = find_receiver(object->type, event);
    if (index >= 0) {
        CExoString string = {
            strdup(script),
            strlen(script)
        };
        const struct Reciever *receiver = receivers + index;
        receiver->set(CAST(object, receiver), event, &string);

        return 0;
    }
    return -1;
}

const char *
get_script(CGameObject *object, int event) {
    const int index = find_receiver(object->type, event);
    if (index >= 0) {
        const struct Reciever *receiver = receivers + index;
        return receiver->get(CAST(object, receiver), event)->text;
    }
    return NULL;
}
