#include "player.h"
#include "simple_logger.h"
#include <stdlib.h>
#include <string.h>
#include "gf2d_graphics.h"
#include "fret.h"

static Entity *_fret = NULL;

Entity *fret_new(Vector2D position, Vector4D colorShift)
{
	Entity *self;
	self = gf2d_entity_new();
	
	if (!self)
	{
		slog("failed to allcate a new fret entity");
		return NULL;
	}
	gf2d_line_cpy(self->name, "fret");
	self->parent = NULL;

	self->shape = gf2d_shape_rect(-16, -16, 32, 32);
	gf2d_body_set(
		&self->body,
		"fret",
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

	gf2d_actor_load(&self->actor, "actors/fretR.actor");
	gf2d_actor_set_action(&self->actor, "idle");
	//self->sound[0] = gf2d_sound_load("sounds/laneSwitch1.wav", 1, -1);

	vector2d_copy(self->position, position);

	vector2d_copy(self->scale, self->actor.al->scale);
	vector2d_set(self->scaleCenter, 32, 32);
	vector4d_copy(self->actor.color, colorShift);
	self->update = fret_update;

	self->health = self->maxHealth = 100;
	_fret = self;
	return self;

}

void fret_update(Entity *self)
{
	if (!(int)gf2d_graphics_get_frame_diff())
		self->position.y += 2;
	self->body.position = self->position;
	gf2d_body_draw(&self->body, vector2d(0, 0));
}

void fret_free(Entity *self)
{
	int i;
	if (!self)return;
	if (self->free)self->free(self);
	for (i = 0; i < EntitySoundMax; i++)
	{
		gf2d_sound_free(self->sound[i]);
	}
	gf2d_actor_free(&self->actor);
	gf2d_particle_emitter_free(self->pe);
	memset(self, 0, sizeof(Entity));
}
