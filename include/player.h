#ifndef __PLAYER_H___
#define __PLAYER_H___

#include "gf2d_entity.h"
#include "gf2d_vector.h"
#include "gf2d_text.h"
#include "simple_json.h"
#include "gf2d_space.h"

enum Lane { BOT, MID, TOP };
int score;
typedef struct
{
	enum Lane currentLane;
}PlayerData;

/**
 * @brief Creates a new Entity that is a Player
 * @param position where fret starts
 * @return NULL on error or a pointer to a Entity
 */
Entity *player_new(Vector2D position);
/**
 * @brief Updates player score and position
 * @param pointer to player
 */
void player_update(Entity *self);
/**
 * @brief concates chars
 * @param char 1
 * @param char 2
 * @return char concated of char 1 and char 2
 */
char* concat(const char *s1, const char *s2);
#endif