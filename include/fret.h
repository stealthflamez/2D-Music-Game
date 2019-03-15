#ifndef __FRET_H___
#define __FRET_H___

#include "gf2d_entity.h"
#include "gf2d_vector.h"

Entity *fret_new(Vector2D position, char *type);
void fret_update(Entity *self);
#endif