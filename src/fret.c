#include "player.h"
#include "simple_logger.h"
#include <stdlib.h>
#include <string.h>

static Entity *_fret = NULL;

void fret_update(Entity *self);
int  fret_touch(Entity *self, Entity *other);

Entity *fret_new(Vector2D position, char *type)
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

	self->shape = gf2d_shape_rect(-27, -27, 54, 54);
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
	/*
	char actorType = { 0 };
	char result = malloc(strlen(actorType) + strlen("actors/") + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, actorType);
	strcat(result, "actors/");
	slog(result);
	free(actorType);
	free(result);


	actorType = strcat(actorType, "actors/");
	actorType = strcat(actorType, type);
	strcat(actorType, ".actor");
	strcat(actorType, "\0");
	slog(actorType);

*/

	gf2d_actor_load(&self->actor, "actors/fretR.actor");
	gf2d_actor_set_action(&self->actor, "idle");
	//self->sound[0] = gf2d_sound_load("sounds/laneSwitch1.wav", 1, -1);

	vector2d_copy(self->position, position);

	vector2d_copy(self->scale, self->actor.al->scale);
	vector2d_set(self->scaleCenter, 27, 27);
	vector3d_set(self->rotation, 27, 27, 0);
	vector2d_set(self->flip, 1, 0);

	//self->draw = player_draw;
	self->update = fret_update;
	//self->touch = player_touch;

	self->health = self->maxHealth = 100;
	_fret = self;
	//level_add_entity(self);
	return self;

}

void fret_update(Entity *self)
{
	self->position.y += 2;
	self->body.position = self->position;
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
