#include "player.h"
#include "simple_logger.h"
#include <stdlib.h>
#include <string.h>
#include "gf2d_graphics.h"
#include "scratch.h"

static Entity *_scratch = NULL;

Entity *scratch_new(Vector2D position, Vector4D colorShift)
{
	Entity *self;
	self = gf2d_entity_new();

	if (!self)
	{
		slog("failed to allcate a new scratch entity");
		return NULL;
	}
	gf2d_line_cpy(self->name, "scratch");
	self->parent = NULL;

	self->shape = gf2d_shape_rect(-25, -25, 50, 50);
	gf2d_body_set(
		&self->body,
		"scratch",
		1,
		1,
		1,
		1,
		position,
		vector2d(0, 0),
		0,
		0,
		0,
		&self->shape,
		NULL,
		NULL);

	gf2d_actor_load(&self->actor, "actors/scratch.actor");
	gf2d_actor_set_action(&self->actor, "idle");
	//self->sound[0] = gf2d_sound_load("sounds/laneSwitch1.wav", 1, -1);

	vector2d_copy(self->position, position);

	vector2d_copy(self->scale, self->actor.al->scale);
	//Center needs to be center of image, not shape size
	vector2d_set(self->scaleCenter, 240, 240);
	vector4d_copy(self->actor.color, colorShift);
	self->update = scratch_update;
	_scratch = self;
	return self;

}

void scratch_update(Entity *self)
{
	self->position.y += 4;
	self->body.position = self->position;
	//	gf2d_body_draw(&self->body, vector2d(0, 0));
}