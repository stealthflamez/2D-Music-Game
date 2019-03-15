
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

void writeTrack(char *filename, List *TrackNotes )
{
	FILE *file;
	file = fopen(filename, "w");
	if (!file)
	{
		return;
	}
	for (int i = 0; i < TrackNotes->count; i++)
	{
		Entity *f;
		f = TrackNotes->elements[i].data;
		fprintf_s(file, "%i\n%i\n", (int)f->position.x, (int)f->position.y);
	}

	fclose(file);
}

void loadTrack(char *filename, List *TrackNotes)
{
	FILE *file;
	file = fopen(filename, "r");
	double x;
	double y;
	if (!file)
	{
		return;
	}
	while (!feof(file))
	{
		fscanf(file, "%d", &x);
		fscanf(file, "%d", &y);
		slog("%d", x);
		slog("%d", y);
	}
	fclose(file);
}

int main(int argc, char * argv[])
{
	/*variable declarations*/
	int i;
	int fullscreen = 0;
	Sprite *background = NULL;

	//jeff
	Entity *player;
	const Uint8 * keys;
	Space *space = NULL;
	Mix_Music *music;
	//Collision *col;

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
	// game specific setup
	//jeff
	space = gf2d_space_new_full(
		3,
		gf2d_rect(0, 0, 1200, 700),
		0.1,
		vector2d(0, 0, 1),
		1,
		20);

	player = player_new(vector2d(600, 650));	
	gf2d_space_add_body(space, &player->body);

	music = Mix_LoadMUS("music/track1.mp3");
	Mix_PlayMusic(music, -1);
	//	Entity *track[500];
	List *track = NULL;
	track = gf2d_list_new();

	////writes fret to track

	//for (int i = 0; i <= 10; i++)
	//{
	//	slog("%i", i);
	//	track = gf2d_list_append(track, fret_new(vector2d(1200 + i * 75, 160), "fretR"));
	//}

	// middle track
	//for ( i = 0 ; i <= 10 ; i++)
	//{
	//	slog("%i",i);
	//	track[i] = fret_new(vector2d(1000 + i * 100, 360), "fretG");
	//	gf2d_space_add_body(space, &track[i]->body);
	//}
	//// bottom track
	//for (i = 0; i <= 10; i++)
	//{
	//	slog("%i", i);
	//	track[i] = fret_new(vector2d(700 + i * 25, 560), "fretB");
	//	gf2d_space_add_body(space, &track[i]->body);
	//}
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
		gf2d_entity_update_all();
		// Draw entities
		//jeff
		//collision beteen fret and player
		//hitNote(track, player);

		if (gf2d_input_key_pressed(" "))
		{
			track = gf2d_list_append(track, (fret_new(player->position, "fretR")));
		}
		if (gf2d_input_key_pressed("9"))
		{
			writeTrack("track1", track);
		}
		if (gf2d_input_key_pressed("8"))
		{
			loadTrack("track1", track);
		}
		

		//slog("%i", gf2d_body_body_collide(&player->body, base	));
		gf2d_body_draw(&player->body, vector2d(0, 0));
		 //gf2d_shape_draw(player->shape, gf2d_color(255, 255, 0, 255), player->position);
		 //gf2d_shape_draw(fret->shape, gf2d_color(255, 255, 0, 255), fret->position);
		
		gf2d_entity_draw_all();
		//UI elements last
		gf2d_windows_draw_all();
		gf2d_mouse_draw();
		gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

		if ((gf2d_input_command_down("exit")) && (_quit == NULL))
		{
			_quit = window_yes_no("Exit?", onExit, onCancel, NULL, NULL);
		}
		//     slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
	}
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	slog("---==== END ====---");
	return 0;
}
/*eol@eof*/
