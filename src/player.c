#include "player.h"
#include "simple_logger.h"
#include "gf2d_input.h"
#include <stdlib.h>
#include <string.h>
#include "gf2d_collision.h"
#include "gf2d_body.h"

static Entity *_player = NULL;

static PlayerData playerData = {
	MID
};

Entity *player_new(Vector2D position)
{
	Entity *self;
	self = gf2d_entity_new();
	if (!self)
	{
		slog("failed to allcate a new player entity");
		return NULL;
	}
	gf2d_line_cpy(self->name, "player");
	self->parent = NULL;
	
	self->shape = gf2d_shape_rect(-27,-27, 54, 54);
	gf2d_body_set(
		&self->body,
		"player",
		1,
		1,
		1,
		2,
		position,
		vector2d(0,0),
		0,
		0,
		0,
		&self->shape,
		NULL,
		NULL);

	gf2d_actor_load(&self->actor, "actors/player.actor");
	gf2d_actor_set_action(&self->actor, "idle");

	//self->sound[0] = gf2d_sound_load("sounds/laneSwitch1.wav", 1, -1);

	vector2d_copy(self->position, position);

	vector2d_copy(self->scale, self->actor.al->scale);
	vector2d_set(self->scaleCenter, 270, 270);
//vector3d_set(self->rotation, 64, 64, 0);

	//self->draw = player_draw;
	self->update = player_update;
	//self->touch = player_touch;

	self->data = (void*)&playerData;
	self->health = self->maxHealth = 100;
	_player = self;
	//level_add_entity(self);
	return self;

}



void player_update(Entity *self)
{
	
}
