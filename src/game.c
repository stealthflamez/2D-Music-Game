#include "game.h"
const int JOYSTICK_DEAD_ZONE = 8000;
static int _done = 0;
static Window *_quit = NULL;
static float hideText = 0;
static int level = 0;
static int tracknum = 0;
static List *player = NULL;
static List *track = NULL;
static Mix_Music *music;
static int mode = 0;
static int endtime = 0;
static ParticleEmitter *pe;
SDL_Joystick* gGameController = NULL;
int HeldR;
int HeldG;
int HeldB;
int Held;
int turning;
SDL_Event e;
int currentlane;
char* text;
char buf[33];

int score;

void onCancel(void *data)
{
	_quit = NULL;
}
void onExit(void *data)
{
	_done = 1;
	_quit = NULL;
}

char* concat(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
void hitNoteFX(Entity *player)
{
	Shape shape;
	shape = gf2d_shape_circle(0, 0, 8);

	pe = gf2d_particle_emitter_new_full(
		10,
		100,
		5,
		PT_Sprite,
		player->body.position,
		vector2d(2, 2),
		vector2d(0, -3),
		vector2d(2, 1),
		vector2d(0, 0.05),
		vector2d(0, 0.01),
		gf2d_color(0.85, 0.55, 0, 1),
		gf2d_color(-0.01, -0.02, 0, 0),
		gf2d_color(0.1, 0.1, 0, 0.1),
		&shape,
		0,
		1,
		1,
		"images/flameP.png",
		81,
		123,
		80,
		60,
		//        SDL_BLENDMODE_BLEND);
		SDL_BLENDMODE_ADD);
	
	gf2d_particle_new_default(pe, 20);
}
void hitNote(List *track, Entity *player)
{
		for (int i = 0; i < track->count; i++)
		{
			Entity *f;
			f = (Shape*)track->elements[i].data;
			if (gf2d_body_body_collide(&player->body, &f->body))
			{
				slog("hit");
				hitNoteFX(player);
				score += 100;
				itoa(score, buf, 10);
				text = concat("score: ", buf);
				gf2d_entity_free(track->elements[i].data);
				gf2d_list_delete_nth(track, i);
				break;
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
		fprintf_s(file, "%f\n%f\n", f->position.x, f->position.y);
	}

	fclose(file);
}

void SaveHighScore()
{
	FILE *file;
	char buf[33];
	itoa(tracknum, buf, 10);
	char* trackname = concat("HighScore for track ", buf);
	file = fopen(trackname, "a");
	if (!file)
	{
		return;
	}
	fprintf_s(file, "%i\n", score);
	fclose(file);
}

void writeTrack(List *track)
{
	if (gf2d_input_key_pressed("9"))
	{
		char buf[33];
		itoa(tracknum, buf, 10);
		char* trackname = concat("track", buf);
		writeTrackToFile(trackname, track);
	}
}

List *loadTrackFromFile(char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	List *track = NULL;
	track = gf2d_list_new_size(500);
	float x;
	float y;
	float i;
	float offset;
	if (!file)
	{
		return;
	}
	fscanf(file, "%f", &x);
	fscanf(file, "%f", &offset);
	while (!feof(file))
	{
		fscanf(file, "%f", &x);
		fscanf(file, "%f", &y);
		slog("%f", x);
		slog("%f", y);
		if(x <= 500)
			track = gf2d_list_append(track, fret_new(vector2d(x, y - offset + 600), vector4d(0, 255, 0, 255)));
		else if (x == 600)
			track = gf2d_list_append(track, fret_new(vector2d(x, y - offset + 600), vector4d(255, 0, 0, 255)));
		else if (x >= 700)
			track = gf2d_list_append(track, fret_new(vector2d(x, y - offset + 600), vector4d(0, 0, 255, 255)));
	}
	fclose(file);
	return track;
}

void setupLevel(int tracknum)
{

	player = gf2d_list_new_size(3);
	gf2d_list_append(player, player_new(vector2d(500, 650)));
	gf2d_list_append(player, player_new(vector2d(600, 650)));
	gf2d_list_append(player, player_new(vector2d(700, 650)));

	switch (tracknum)
	{
	case 1:
		music = Mix_LoadMUS("music/track1.mp3");
		track = loadTrackFromFile("track1");
		Mix_PlayMusic(music, 0);
		endtime = SDL_GetTicks() + 290000;
		mode++;
		break;
	case 2:
		music = Mix_LoadMUS("music/track2.mp3");
		track = loadTrackFromFile("track2");
		Mix_PlayMusic(music, 0);
		endtime = SDL_GetTicks()+ 212000;
		mode++;
		break;
	case 3:
		music = Mix_LoadMUS("music/track3.mp3");
		track = loadTrackFromFile("track3");
		Mix_PlayMusic(music, 0);
		endtime = SDL_GetTicks() + 62000;
		mode++;
		break;

	default:
		slog("error on track");
		break;
	}
}

void setupWriteLevel(int tracknum)
{
	player = gf2d_list_new_size(3);
	gf2d_list_append(player, player_new(vector2d(500, 650)));
	gf2d_list_append(player, player_new(vector2d(600, 650)));
	gf2d_list_append(player, player_new(vector2d(700, 650)));

	switch (tracknum)
	{
	case 1:
		music = Mix_LoadMUS("music/track1.mp3");
		Mix_PlayMusic(music, 0);
		gf2d_list_delete(track);
		track = gf2d_list_new_size(500);
		track = gf2d_list_append(track, (fret_new(vector2d(600, 650), vector4d(255, 0, 0, 255))));
		endtime = SDL_GetTicks() + 290000;
		mode = 2;
		break;
	case 2:
		music = Mix_LoadMUS("music/track2.mp3");
		Mix_PlayMusic(music, 0);
		gf2d_list_delete(track);
		track = gf2d_list_new_size(500);
		track = gf2d_list_append(track, (fret_new(vector2d(600, 650), vector4d(255, 0, 0, 255))));
		endtime = SDL_GetTicks() + 212000;
		mode = 2;
		break;
	case 3:
		music = Mix_LoadMUS("music/track3.mp3");
		Mix_PlayMusic(music, 0);
		gf2d_list_delete(track);
		track = gf2d_list_new_size(500);
		track = gf2d_list_append(track, (fret_new(vector2d(600, 650), vector4d(255, 0, 0, 255))));
		endtime = SDL_GetTicks() + 62000;
		mode = 2;
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
			gf2d_text_draw_line("1)Lil Jon(Get Low) vs. 50 Cent(In Da Club)", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			gf2d_text_draw_line("2)Love Is Gone vs. Black & Gold", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 200));
			gf2d_text_draw_line("3)track 3", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 300));


			if (e.type == SDL_JOYAXISMOTION)
			{
				//Motion on controller 0
				if (e.jaxis.which == 0)
				{
					if (e.jaxis.axis == 1)
					{
						if (e.jaxis.value > 8 && turning == 0)
						{
							slog("down G");
							turning = 1;
						}
						else if (e.jaxis.value < -8 && turning == 0)
						{
							slog("up G");
							turning = 1;
						}
						else {
							turning = 0;
						}
					}
				}
			}
			if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == 0 && HeldG == 0)
				{
					//a green fret
					slog("hit green button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 0));
					HeldG = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 1 && HeldR == 0)
				{
					//b red
					slog("hit red button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 1));
					HeldR = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 2 && HeldB == 0)
				{
					//x blue
					slog("hit blue button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 2));
					HeldB = 1;
					/* code goes here */
				}
			}
			if (e.type == SDL_JOYBUTTONUP)
			{
				HeldG = 0;
				HeldB = 0;
				HeldR = 0;
			}


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
			else if (gf2d_input_key_pressed("3"))
			{
				level++;
				tracknum = 3;
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
				setupWriteLevel(tracknum);
			}

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

	itoa(score, buf, 10);
	text = concat("score: ", buf);

	//basic setup
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
	gf2d_audio_init(256, 16, 4, 1, 1, 1);
	gf2d_sprite_init(1024);
	gf2d_action_list_init(128);
	gf2d_text_init("config/font.cfg");
	gf2d_input_init("config/input.cfg");
	gf2d_windows_init(128);
	gf2d_entity_system_init(1024);

	camera_set_dimensions(0, 0, 1200, 700);
	background = gf2d_sprite_load_all(
		"images/backgrounds/bgf.png",
		600,
		338,
		5,
		false
	);

	SDL_ShowCursor(SDL_DISABLE);

	// init mouse, editor window
	gf2d_mouse_load("actors/mouse.actor");
	int frame = 0;
	int loop = 80;
	Vector2D *scale = vector2d_new();
	Vector2D *scaleC = vector2d_new();
	scale->x = 2 * 1.5;
	scale->y = 2.13 * 1.5;
	scaleC->x = 300;
	scaleC->y = 169;
	Vector4D colorS;
	vector4d_set(colorS, (float)rand() / (float)255, (float)rand() / (float)255, (float)rand() / (float)255, 255);

	//Check for joysticks
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else
	{
		//Load joystick
		gGameController = SDL_JoystickOpen(0);
		if (gGameController == NULL)
		{
			printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
		}
	}
	
	int number_of_buttons;
	float slider;
	

	//Shape line = gf2d_shape_edge(0,0, 300,300);
	//new player system
	
	
	/*
	Entity *f = (Entity*)gf2d_list_get_nth(player, 0);
	f->position = vector2d(500, 200);
	f->body.position = vector2d(500, 200);
	//gf2d_list_insert(player, f, 0);*/
	while (SDL_PollEvent(&e) != 0 || !_done)
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
		
		gf2d_sprite_draw(background, vector2d(600, 350), scale, scaleC, NULL, NULL, &colorS, (frame / 3) % 80);
		if (frame > 240)
		{
			vector4d_set(colorS, (float)rand() / (float)255, (float)rand() / (float)255, (float)rand() / (float)255, 255);
			frame = 0;
		}
			
		frame++;
		// DRAW WORLD
		Menu();
		gf2d_entity_draw_all();
		
		switch (mode)
		{
		case 1:
			if (e.type == SDL_JOYAXISMOTION)
			{
				//Motion on controller 0
				if (e.jaxis.which == 0)
				{
					//X axis motion
					if (e.jaxis.axis == 4)
					{
						//Left of dead zone
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							slog("one");
							//right
							if (currentlane != 0)
							{
								Entity *w = (Entity*)gf2d_list_get_nth(player, 2);
								w->position = vector2d(800, 650);
								w->body.position = vector2d(800, 650);
								//gf2d_list_insert(player, w, 0);
								currentlane = 0;
							}
						}
						//Right of dead zone
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							slog("two");
							//right
							if (currentlane != 1)
							{
								Entity *q = (Entity*)gf2d_list_get_nth(player, 0);
								q->position = vector2d(400, 650);
								q->body.position = vector2d(400, 650);
								//gf2d_list_insert(player, q, 2);
								currentlane = 1;
							}
						}
						else
						{
							//middle
							if (currentlane != 2)
							{
								Entity *f = (Entity*)gf2d_list_get_nth(player, 2);
								f->position = vector2d(700, 650);
								f->body.position = vector2d(700, 650);
								//gf2d_list_insert(player, f, 1);

								Entity *g = (Entity*)gf2d_list_get_nth(player, 0);
								g->position = vector2d(500, 650);
								g->body.position = vector2d(500, 650);
								//gf2d_list_insert(player, g, 2);
								currentlane = 2;
							}
						}
					}
				}
				if (e.jaxis.axis == 1)
				{
					if (e.jaxis.value > 4 && HeldG)
					{
						slog("down G");
					}
					if (e.jaxis.value < -4 && HeldG)
					{
						slog("up G");
					}
					if (e.jaxis.value > 4 && HeldR)
					{
						slog("down R");
					}
					if (e.jaxis.value < -4 && HeldR)
					{
						slog("up R");
					}
					if (e.jaxis.value > 4 && HeldB)
					{
						slog("down B");
					}
					if (e.jaxis.value < -4 && HeldB)
					{
						slog("up B");
					}
				}
			}
			if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == 0 && HeldG == 0)
				{
					//a green fret
					slog("hit green button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 0));
					HeldG = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 1 && HeldR == 0)
				{
					//b red
					slog("hit red button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 1));
					HeldR = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 2 && HeldB == 0)
				{
					//x blue
					slog("hit blue button");
					hitNote(track, (Entity*)gf2d_list_get_nth(player, 2));
					HeldB = 1;
					/* code goes here */
				}
			}
			if (e.type == SDL_JOYBUTTONUP)
			{
				HeldG = 0;
				HeldB = 0;
				HeldR = 0;
			}
			gf2d_text_draw_line(text, FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
			gf2d_particle_emitter_update(pe);
			gf2d_particle_emitter_draw(pe);

			if (SDL_GetTicks() > endtime)
			{
				SaveHighScore();
				mode = 0;
				level = 0;
				Mix_HaltMusic();
				Mix_FreeMusic(music);
				score = 0;
				tracknum = 0;
				endtime = 0;
				gf2d_list_delete(player);
			}
			break;
		case 2:
			if (e.type == SDL_JOYAXISMOTION)
			{
				//Motion on controller 0
				if (e.jaxis.which == 0)
				{
					//X axis motion
					if (e.jaxis.axis == 4)
					{
						//Left of dead zone
						if (e.jaxis.value < -JOYSTICK_DEAD_ZONE)
						{
							slog("one");
							//right
							if (currentlane != 0)
							{
								Entity *w = (Entity*)gf2d_list_get_nth(player, 2);
								w->position = vector2d(800, 650);
								w->body.position = vector2d(800, 650);
								//gf2d_list_insert(player, w, 0);
								currentlane = 0;
							}
						}
						//Right of dead zone
						else if (e.jaxis.value > JOYSTICK_DEAD_ZONE)
						{
							slog("two");
							//right
							if (currentlane != 1)
							{
								Entity *q = (Entity*)gf2d_list_get_nth(player, 0);
								q->position = vector2d(400, 650);
								q->body.position = vector2d(400, 650);
								//gf2d_list_insert(player, q, 2);
								currentlane = 1;
							}
						}
						else
						{
							//middle
							if (currentlane != 2)
							{
								Entity *f = (Entity*)gf2d_list_get_nth(player, 2);
								f->position = vector2d(700, 650);
								f->body.position = vector2d(700, 650);
								//gf2d_list_insert(player, f, 1);

								Entity *g = (Entity*)gf2d_list_get_nth(player, 0);
								g->position = vector2d(500, 650);
								g->body.position = vector2d(500, 650);
								//gf2d_list_insert(player, g, 2);
								currentlane = 2;
							}
						}
					}
				}
			}
			if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == 0 && HeldG == 0)
				{
					//a green fret
					slog("hit green button");
					Entity *g = (Entity*)gf2d_list_get_nth(player, 0);
					track = gf2d_list_append(track, (fret_new(g->position, vector4d(0, 255, 0, 255))));
					HeldG = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 1 && HeldR == 0)
				{
					//b red
					slog("hit red button");
					Entity *f = (Entity*)gf2d_list_get_nth(player, 1);
					track = gf2d_list_append(track, (fret_new(f->position, vector4d(255, 0, 0, 255))));
					HeldR = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 2 && HeldB == 0)
				{
					//x blue
					slog("hit blue button");
					Entity *q = (Entity*)gf2d_list_get_nth(player, 2);
					track = gf2d_list_append(track, (fret_new(q->position, vector4d(0, 0, 255, 255))));
					HeldB = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 7 && Held == 0)
				{
					//start
					char buf[33];
					itoa(tracknum, buf, 10);
					char* trackname = concat("track", buf);
					writeTrackToFile(trackname, track);
					Held = 1;
				}
				
			}
			if (e.type == SDL_JOYBUTTONUP)
			{
				HeldG = 0;
				HeldB = 0;
				HeldR = 0;
				Held = 0;
			}
			writeTrack(track);
			if (SDL_GetTicks() > endtime)
			{
				char buf[33];
				itoa(tracknum, buf, 10);
				char* trackname = concat("track", buf);
				writeTrackToFile(trackname, track);
				mode = 0;
				level = 0;
				Mix_HaltMusic();
				Mix_FreeMusic(music);
				score = 0;
				tracknum = 0;
				endtime = 0;
				gf2d_list_delete(player);
			}
		default:
			break;
		}

		//line to each note
		//gf2d_shape_draw(line, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
		gf2d_entity_update_all();
	
		gf2d_windows_draw_all();
		//gf2d_mouse_draw();
		gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
	}
	Mix_HaltMusic();
	Mix_FreeMusic(music);
	slog("---==== END ====---");
	return 0;
}
/*eol@eof*/
