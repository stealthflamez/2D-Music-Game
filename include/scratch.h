#ifndef __SCRATCH_H___
#define __SCRATCH_H___

#include "gf2d_entity.h"
#include "gf2d_vector.h"

/**
 * @brief Creates a new Entity that is a scratch
 * @param position where fret starts
 * @param what type of fret it is
 * @return NULL on error or a pointer to a Entity
 */
Entity *scratch_new(Vector2D position, Vector4D colorShift);
/**
 * @brief Updates scratch
 * @param pointer to fret
 */
void scratch_update(Entity *self);
/**
 * @brief frees fret
 * @param pointer to fret
 */
#endif