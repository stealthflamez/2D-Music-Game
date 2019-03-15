#include "level.h"
#include "camera.h"
//#include "spawn.h"
#include "player.h"
//#include "entity_common.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_particles.h"
#include "gf2d_config.h"
#include <stdio.h>

typedef struct
{
	Space           *space;
	Sprite          *backgroundImage;
	Sprite          *tileLayer;
	Sprite          *tileSet;
	Mix_Music       *backgroundMusic;
	ParticleEmitter *pe;
}Level;

static Level gamelevel = { 0 };

int *level_alloc_tilemap(int w, int h);

void level_clear()
{
	gf2d_space_free(gamelevel.space);
	gf2d_sprite_free(gamelevel.backgroundImage);
	gf2d_sprite_free(gamelevel.tileSet);
	gf2d_sprite_free(gamelevel.tileLayer);
	gf2d_particle_emitter_free(gamelevel.pe);
	if (gamelevel.backgroundMusic)
	{
		Mix_FreeMusic(gamelevel.backgroundMusic);
	}
	memset(&gamelevel, 0, sizeof(Level));
	slog("level cleared");
}

void level_info_free(LevelInfo *linfo)
{
	if (!linfo)return;
	if (linfo->tileMap != NULL)
	{
		free(linfo->tileMap);
	}
	sj_free(linfo->spawnList);
	//TODO:free spawn list
	free(linfo);
}

LevelInfo *level_info_new()
{
	LevelInfo *linfo = NULL;
	linfo = (LevelInfo*)malloc(sizeof(LevelInfo));
	if (!linfo)
	{
		slog("failed to allocate level info");
		return NULL;
	}
	memset(linfo, 0, sizeof(LevelInfo));

	level_setup();

	return linfo;
}

void level_make_space()
{
	gamelevel.space = gf2d_space_new_full(
		3,
		gf2d_rect(0, 0, 1200, 700),
		0.1,
		vector2d(0, 0, 1),
		1,
		20);
}

void level_setup() 
{
	Entity *player;
	Entity *fret;
	level_make_space();
	player = player_new(vector2d(150, 360));
	fret = fret_new(vector2d(1200, 360));
	gf2d_space_add_body(gamelevel.space, &player->body);
	gf2d_space_add_body(gamelevel.space, &fret->body);
}

Space *level_get_space()
{
	return gamelevel.space;
}