#include "level.h"
#include "camera.h"
#include "player.h"
#include "simple_json.h"
#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_particles.h"
#include "gf2d_config.h"
#include "windows_common.h"
#include "gf2d_input.h"
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
	

	level_make_space();
	//music = Mix_LoadMUS("music/track2.mp3");
	//Mix_HaltMusic();
	//Mix_FreeMusic(music);
}






