
#include <SDL.h>
#include <stdio.h>
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_input.h"
#include "gf2d_audio.h"
#include "gf2d_windows.h"
#include "gf2d_entity.h"
#include "gf2d_mouse.h"
#include "simple_logger.h"
#include "windows_common.h"
#include "gf2d_space.h"

//jeff
#include "player.h"
#include "fret.h"
#include "level.h"

static int _done = 0;
static Window *_quit = NULL;
static float hideText = 0;
static int level = 0;
static int tracknum = 0;
static Entity *player;
static List *track = NULL;
static Mix_Music *music;
static int mode = 0;

void onCancel(void *data)
{
	_quit = NULL;
}
void onExit(void *data)
{
	_done = 1;
	_quit = NULL;
}

void hitNote(List *track, Entity *player)
{
	if (gf2d_input_key_pressed(" "))
	{
		for (int i = 0; i < track->count; i++)
		{
			Entity *f;
			f = (Shape*)track->elements[i].data;
			if (gf2d_body_body_collide(&player->body, &f->body))
			{
				slog("hit");
				score += 100;
				gf2d_entity_free(track->elements[i].data);
				gf2d_list_delete_nth(track, i);
				break;
			}
		}
	}
}

void writeTrackToFile(char *filename, List *track)
{
	FILE *file;
	file = fopen(filename, "w");
	if (!file)
	{
		return;
	}
	for (int i = 0; i < track->count; i++)
	{
		Entity *f;
		f = track->elements[i].data;
		fprintf_s(file, "%i\n%i\n", (int)f->position.x, (int)f->position.y);
	}

	fclose(file);
}

void writeTrack(List *track)
{
	if (gf2d_input_key_pressed("1"))
	{
		track = gf2d_list_append(track, (fret_new(vector2d(600, 650), "fretR")));
	}/*
	else if (gf2d_input_key_pressed("2"))
	{
		track = loadTrackFromFile("track1");
	}*/
	else if (gf2d_input_key_pressed("z"))
	{
		track = gf2d_list_append(track, (fret_new(vector2d(400, 650), "fretR")));
	}
	else if (gf2d_input_key_pressed("x"))
	{
		track = gf2d_list_append(track, (fret_new(vector2d(600, 650), "fretR")));
	}
	else if (gf2d_input_key_pressed("c"))
	{
		track = gf2d_list_append(track, (fret_new(vector2d(800, 650), "fretR")));
	}
	else if (gf2d_input_key_pressed("9"))
	{
		writeTrackToFile("track1", track);
	}
}

List *loadTrackFromFile(char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	List *track = NULL;
	track = gf2d_list_new_size(500);
	int x;
	int y;
	int i;
	int offset;
	if (!file)
	{
		return;
	}
	fscanf(file, "%i", &x);
	fscanf(file, "%i", &offset);
	while (!feof(file))
	{
		fscanf(file, "%i", &x);
		fscanf(file, "%i", &y);
		slog("%i", x);
		slog("%i", y);
		track = gf2d_list_append(track, fret_new(vector2d(x, y - offset), "fretR"));
	}
	fclose(file);
	return track;
}

int setupLevel(int tracknum)
{
	//
	player = player_new(vector2d(600, 650));

	switch (tracknum)
	{
	case 1:
		music = Mix_LoadMUS("music/track1.mp3");
		track = loadTrackFromFile("track1");
		Mix_PlayMusic(music, -1);
		mode++;
		break;
	case 2:
		music = Mix_LoadMUS("music/track2.mp3");
		track = loadTrackFromFile("track2");
		Mix_PlayMusic(music, -1);
		mode++;
		break;
	default:
		slog("error on track");
		break;
	}
}

void Menu()
{
	switch (level)
	{
		case 0:
			gf2d_text_draw_line("Tracks", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
			gf2d_text_draw_line("1:)Lil Jon(Get Low) vs. 50 Cent(In Da Club)", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			gf2d_text_draw_line("2:)Love Is Gone vs. Black & Gold", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 200));
			if (gf2d_input_key_pressed("1"))
			{
				level++;
				tracknum = 1;
			}
			else if (gf2d_input_key_pressed("2"))
			{
				level++;
				tracknum = 2;
			}
			break;
		case 1:
			gf2d_text_draw_line("1:)PLay song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
			gf2d_text_draw_line("2:)Write song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			if (gf2d_input_key_pressed("1"))
			{
				level++;
				setupLevel(tracknum);
			}
			else if (gf2d_input_key_pressed("2"))
			{
				level++;
			}
		case 2:

	default:
		break;
	}
}

int main(int argc, char * argv[])
{
	/*variable declarations*/
	int i;
	int fullscreen = 0;
	Sprite *background = NULL;

	
	const Uint8 * keys;
	Space *space = NULL;

	//basic setup
	

	//player = player_new(vector2d(600, 650));

	/*parse args*/
	for (i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "--fullscreen") == 0)
		{
			fullscreen = 1;
		}
	}

	/*program initializtion*/
	init_logger("gf2d.log");
	slog("---==== BEGIN ====---");
	gf2d_graphics_initialize(
		"gf2d",
		1200,
		720,
		1200,
		720,
		vector4d(0, 0, 0, 255),
		fullscreen);
	gf2d_graphics_set_frame_delay(16);
	gf2d_audio_init(256, 16, 4, 1, 1, 1);
	gf2d_sprite_init(1024);
	gf2d_action_list_init(128);
	gf2d_text_init("config/font.cfg");
	gf2d_input_init("config/input.cfg");
	gf2d_windows_init(128);
	gf2d_entity_system_init(1024);

	camera_set_dimensions(0, 0, 1200, 700);
	background = gf2d_sprite_load_all(
		"images/backgrounds/bg.png",
		435,
		250,
		10,
		false
	);

	SDL_ShowCursor(SDL_DISABLE);
	
	// init mouse, editor window
	gf2d_mouse_load("actors/mouse.actor");
	int frame = 0;
	int loop = 80;
	Vector2D *scale = vector2d_new();
	scale->x = 2.75;
	scale->y = 2.9;
	/*main game loop*/
	while (!_done)
	{
		gf2d_input_update();
		/*update things here*/
		gf2d_windows_update_all();
		gf2d_space_update(space);
		gf2d_entity_think_all();
		gf2d_mouse_update();

		gf2d_graphics_clear_screen();// clears drawing buffers
		// all drawing should happen betweem clear_screen and next_frame
			//backgrounds drawn first
		gf2d_sprite_draw(background, vector2d(0, 0), scale,NULL, NULL, NULL, NULL,(frame/10)%71);
		frame++;
		// DRAW WORLD
		if (mode)
			hitNote(track, player);
		gf2d_entity_update_all();
		// Draw entities		
		gf2d_entity_draw_all();
		//UI elements last
		gf2d_windows_draw_all();
		gf2d_mouse_draw();
		Menu();
		gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

		if ((gf2d_input_command_down("exit")) && (_quit == NULL))
		{
			_quit = window_yes_no("Exit?", onExit, onCancel, NULL, NULL);
		}
	}
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	slog("---==== END ====---");
	return 0;
}
/*eol@eof*/
