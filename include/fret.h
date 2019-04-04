#ifndef __FRET_H___
#define __FRET_H___

#include "gf2d_entity.h"
#include "gf2d_vector.h"

/**
 * @brief Creates a new Entity that is a fret
 * @param position where fret starts
 * @param what type of fret it is
 * @return NULL on error or a pointer to a Entity
 */
Entity *fret_new(Vector2D position, Vector4D colorShift);
/**
 * @brief Updates fret
 * @param pointer to fret
 */
void fret_update(Entity *self);
/**
 * @brief frees fret
 * @param pointer to fret
 */
void fret_free(Entity *self);
#endif