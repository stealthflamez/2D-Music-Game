#include "player.h"
#include "simple_logger.h"
#include "gf2d_input.h"
#include <stdlib.h>
#include <string.h>
#include "gf2d_collision.h"
#include "gf2d_body.h"

static Entity *_player = NULL;

void player_update(Entity *self);

static PlayerData playerData = {
	MID
};

Entity *player_get()
{
	return _player;
}

void UpdateScore()
{
	
}


void player_set_position(Vector2D position)
{
	if (!_player)
	{
		slog("no player loaded");
		return;
	}
	vector2d_copy(_player->position, position);
	vector2d_copy(_player->body.position, position);
}

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
	
	self->shape = gf2d_shape_rect(0, 0, 20, 20);
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
	vector2d_set(self->scaleCenter, 64, 64);
	vector3d_set(self->rotation, 64, 64, 0);

	//self->draw = player_draw;
	self->update = player_update;
	//self->touch = player_touch;

	self->data = (void*)&playerData;
	self->health = self->maxHealth = 100;
	_player = self;
	//level_add_entity(self);
	return self;

}

char* concat(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void player_update(Entity *self)
{
	char buf[33];
	itoa(score, buf, 10);
	char* text = concat("score: ", buf);
	gf2d_text_draw_line(text, FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
	const Uint8 *keys;
	keys = SDL_GetKeyboardState(NULL);

	if (gf2d_input_key_pressed("a") && playerData.currentLane != TOP)
	{
		gf2d_sound_play(self->sound[0], 0, .5, -1, -1);
		playerData.currentLane += 1;
		self->position.x -= 200;
		vector2d_copy(self->body.position, self->position);
	}
	if (gf2d_input_key_pressed("d") && playerData.currentLane != BOT)
	{
		gf2d_sound_play(self->sound[0], 0, .5, -1, -1);
		playerData.currentLane -= 1;
		self->position.x += 200;
		vector2d_copy(self->body.position, self->position);
	}
}
