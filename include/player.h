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

Entity *player_spawn(Vector2D position, SJson *args);
int  player_touch(Entity *self, Entity *other);
Entity *player_new(Vector2D position);
Entity *player_get();
void player_update(Entity *self);

void UpdateScore(Entity *self);
char* concat(const char *s1, const char *s2);
#endif