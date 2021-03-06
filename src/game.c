#include "game.h"

const int JOYSTICK_DEAD_ZONE = 20000;
static int _done = 0;
static Window *_quit = NULL;
//UI menu
static int level = 0;
//position in tracklist
static int tracknum = 0;
//
//Level setup
static List *player = NULL;
static List *track = NULL;
static List *trackG = NULL;
static List *trackR = NULL;
static List *trackB = NULL;
static List *trackName = NULL;
static Mix_Music *music;
static int mode = 0;
static int endtime = 0;
static ParticleEmitter *pe;
static int currentlane;
static Sprite *flare;
//
//controller
SDL_Joystick* gGameController = NULL;
static int HeldR;
static int HeldG;
static int HeldB;
static int Held;
static int scratchup;
static int scratchdown;
static int turning;
static int selected;
static SDL_Event e;
//

//score
static char* text = "Score: ";
static char* streakT = "Streak: ";
static char* multi = "Multiplier: ";
static char buf[33];
static int Tscore;
static int multiplier;
static int streak;
//
char* concat(const char *s1, const char *s2)
{
	char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}
void hitNoteFX(Entity *player, Entity*fret)
{
	/*gf2d_sprite_draw(
		flare,
		player->body.position,
		vector2d(100, 100),
		NULL,
		NULL,
		NULL,
		&player->actor.color,
		0
	);*/
	myUpdate(pe, player->position, gf2d_color(fret->actor.color.x, fret->actor.color.y, fret->actor.color.z, fret->actor.color.w));
	gf2d_particle_new_default(pe, 10);
}
void hitNote(List *track, Entity *player)
{
	Entity *f;
		for (int i = 0; i < track->count; i++)
		{
			
			f = track->elements[i].data;
			if (gf2d_body_body_collide(&player->body, &f->body))
			{
				slog("hit");
				hitNoteFX(player, f);
				streak += 1;
				if (streak < 40)
					multiplier = (streak / 10) + 1;

				Tscore += 100 * multiplier;

				itoa(Tscore, buf, 10);
				text = concat("Score: ", buf);

				itoa(streak, buf, 10);
				streakT = concat("Streak: ", buf);

				itoa(multiplier, buf, 10);
				multi = concat("multiplier: ", buf);

				gf2d_entity_free(track->elements[i].data);
				gf2d_list_delete_nth(track, i);
				return;
			}
		}
		streak = 0;
		multiplier = 0;
}
//done 
void *writeTrackToFile(char *filename)
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
		slog("%f", f->position.x);
		fprintf_s(file, "%i\n%i\n", (int)f->position.x, (int)f->position.y);
	}
	fclose(file);
}
//done uses tracklist, score, selected
void SaveHighScore()
{
	//FILE *file;
	//char buf[33];
	//itoa(tracknum, buf, 10);
	//char* trackname = concat("HighScore for track ", buf);
	//file = fopen(trackname, "a");
	//if (!file)
	//{
	//	return;
	//}
	//fprintf_s(file, "%i\n", score);
	//fclose(file);
	FILE *file;
	char* trackname = concat("HighScore for ", (char*)gf2d_list_get_nth(trackName, tracknum));
	file = fopen(trackname, "a+");
	if (!file)
	{
		return;
	}
	fprintf_s(file, "%i\n", score);
	free(trackname);
	fclose(file);
}
//Free list
void *loadTrackFromFile(char *filename)
{
	//todo fix this so 3 lane save
	FILE *file;
	file = fopen(filename, "r");
	
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
		if(x <= 500)
			trackG = gf2d_list_append(trackG, fret_new(vector2d(x, y - offset + 600), vector4d(0, 255, 0, 255))); //+ 600
		else if (x == 600)
			trackR = gf2d_list_append(trackR, fret_new(vector2d(x, y - offset + 600), vector4d(255, 0, 0, 255)));
		else if (x >= 700)
			trackB = gf2d_list_append(trackB, fret_new(vector2d(x, y - offset + 600), vector4d(0, 0, 255, 255)));
	}
	
	fclose(file);
}
//Free list
List *loadTrackNameFromFile(char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	List *temp = NULL;
	temp = gf2d_list_new();
	TextLine buffer;
	
	while (fscanf(file, "%[^\n] ", buffer) == 1) // expect 1 successful conversion
	{
		//strdup makes a new pointer
		char* d = strdup(buffer);
		temp = gf2d_list_append(temp, d);
		slog(&buffer);
		// process buffer
	}
	if (feof(file))
	{
		// hit end of file
		slog("track loaded");
	}
	else
	{
		// some other error interrupted the read
	}
	fclose(file);
	return temp;
}

void setupLevel(int tracknum, int d)
{

	//Shape shape;
	//shape = gf2d_shape_circle(0, 0, 8);


	player = gf2d_list_new_size(3);
	player = gf2d_list_append(player, player_new(vector2d(500, 650)));
	player = gf2d_list_append(player, player_new(vector2d(600, 650)));
	player = gf2d_list_append(player, player_new(vector2d(700, 650)));

	pe = gf2d_particle_emitter_new_full(
		5000,
		10,
		5,
		PT_Sprite,
		vector2d(2, 2),
		vector2d(2, 2),
		vector2d(0, -2),
		vector2d(2, -1),
		vector2d(0, 0.08),
		vector2d(0, 0.01),
		gf2d_color(0, 0, 0, 1),
		gf2d_color(0, 0, 0, 1),
		gf2d_color(0, 0, 0, 1),
		NULL,
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

	SDL_PumpEvents();
	SDL_FlushEvent(e.type);
	char *l;
	//make generic.name of track and music track known. missing length of song
	
	l = concat("music", "/");
	l = concat(l, (char*)gf2d_list_get_nth(trackName, tracknum));
	l = concat(l, ".mp3");
	music = Mix_LoadMUS(l);
	trackG = gf2d_list_new();
	trackR = gf2d_list_new();
	trackB = gf2d_list_new();
	if (d == 0)
		loadTrackFromFile((char*)gf2d_list_get_nth(trackName, tracknum));
	else
		loadTrackFromFile(concat((char*)gf2d_list_get_nth(trackName, tracknum), " W"));
	Mix_PlayMusic(music, 1);
	mode++;
}

void setupWriteLevel(int tracknum)
{
	player = gf2d_list_new_size(3);
	player = gf2d_list_append(player, player_new(vector2d(500, 650)));
	player = gf2d_list_append(player, player_new(vector2d(600, 650)));
	player = gf2d_list_append(player, player_new(vector2d(700, 650)));
	SDL_PumpEvents();
	SDL_FlushEvent(e.type);
	char *l;

	l = concat("music", "/");
	l = concat(l, (char*)gf2d_list_get_nth(trackName, tracknum));
	l = concat(l, ".mp3");
	music = Mix_LoadMUS(l);
	track = gf2d_list_new();
	track = gf2d_list_append(track, (fret_new(vector2d(600, 650), vector4d(255, 0, 0, 255))));
	Mix_PlayMusic(music, 1);
	mode = 2;
}
//learn how to pass argumwnts for function pointers
void *musicFinished()
{
	SaveHighScore();
	
	gf2d_list_foreach(player, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackG, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackR, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackB, gf2d_entity_free, NULL);
	if (pe != NULL) {
		gf2d_particle_emitter_free(pe);
		pe = NULL;
	}
	text = "Score: ";
	streakT = "Streak: ";
	multi = "Multiplier: ";
	multiplier = 0;
	streak = 0;
	selected = 0;
	tracknum = 0;
	mode = 0;
	level = 0;
	Tscore = 0;
}

void *musicFinishedExit()
{
	
	gf2d_list_foreach(player, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackG, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackR, gf2d_entity_free, NULL);
	gf2d_list_foreach(trackB, gf2d_entity_free, NULL);
	gf2d_list_foreach(track, gf2d_entity_free, NULL);
	if (pe != NULL) {
		gf2d_particle_emitter_free(pe);
		pe = NULL;
	}
	text = "Score: ";
	streakT = "Streak: ";
	multi = "Multiplier: ";
	multiplier = 0;
	streak = 0;
	tracknum = 0;
	selected = 0;
	mode = 0;
	level = 0;
	Mix_PauseMusic();
	Tscore = 0;
	
}

void *musicFinishedWrite()
{
	writeTrackToFile(concat((char*)gf2d_list_get_nth(trackName, tracknum), " W"));
	gf2d_list_foreach(player, gf2d_entity_free, NULL);
	gf2d_list_foreach(track, gf2d_entity_free, NULL);
	tracknum = 0;
	selected = 0;
	mode = 0;
	level = 0;
}

void Menu()
{
	switch (level)
	{
	case 0:
		//tracklist selection
		gf2d_text_draw_line("Tracks", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
		//slog("%i", selected);
		if (selected > ((int)trackName->count - 1))
			selected = 0;
		else  if (selected < 0)
			selected = ((int)trackName->count - 1);

		for (int i = 0; i < trackName->count; i++)
		{
			if (i == selected)
				gf2d_text_draw_line((char*)gf2d_list_get_nth(trackName, i), FT_H1, gf2d_color(255, 0, 255, 255), vector2d(0, 50 * i + 50));
			else
				gf2d_text_draw_line((char*)gf2d_list_get_nth(trackName, i), FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 50 * i + 50));
		}



		if (e.type == SDL_JOYAXISMOTION)
		{
			//Motion on controller 0

			if (e.jaxis.which == 0)
			{
				//slog("%i", selected);
				if (e.jaxis.axis == 1 && turning == 0)
				{
					//slog("%i", e.jaxis.value);
					if (e.jaxis.value > 6)
					{
						//slog("down G");
						turning = 1;
						selected++;
					}
					else if (e.jaxis.value < -6)
					{
						//slog("up G");
						turning = 1;
						selected--;
					}
				}
				else if (e.jaxis.value == -1)
				{
					turning = 0;
				}

			}
		}
		if (e.type == SDL_JOYBUTTONDOWN)
		{
			if (e.jbutton.button == 0 && HeldG == 0)
			{
				//a green fret
				//slog("hit green button");
				level++;
				tracknum = selected;
				HeldG = 1;
				/* code goes here */
			}
		}
		if (e.type == SDL_JOYBUTTONUP)
		{
			HeldG = 0;
			HeldR = 0;
		}
		break;
	case 1:

		if (selected > 2)
			selected = 0;
		else  if (selected < 0)
			selected = 2;

		for (int i = 0; i < 2; i++)
		{
			if (selected == 0)
			{

				gf2d_text_draw_line("Play song", FT_H1, gf2d_color(255, 0, 255, 255), vector2d(0, 0));
				gf2d_text_draw_line("Write song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 50));
				gf2d_text_draw_line("Play write song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			}
			else if (selected == 1)
			{
				gf2d_text_draw_line("Play song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
				gf2d_text_draw_line("Write song", FT_H1, gf2d_color(255, 0, 255, 255), vector2d(0, 50));
				gf2d_text_draw_line("Play write song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			}
			else if (selected == 2)
			{
				gf2d_text_draw_line("Play song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
				gf2d_text_draw_line("Write song", FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 50));
				gf2d_text_draw_line("Play write song", FT_H1, gf2d_color(255, 0, 255, 255), vector2d(0, 100));
			}
		}

		if (e.type == SDL_JOYAXISMOTION)
		{
			//Motion on controller 0

			if (e.jaxis.which == 0)
			{
				//slog("%i", selected);
				if (e.jaxis.axis == 1 && turning == 0)
				{
					//slog("%i", e.jaxis.value);
					if (e.jaxis.value > 6)
					{
						//slog("down G");
						turning = 1;
						selected++;
					}
					else if (e.jaxis.value < -6)
					{
						//slog("up G");
						turning = 1;
						selected--;
					}
				}
				else if (e.jaxis.value == -1)
				{
					turning = 0;
				}

			}
		}
		if (e.type == SDL_JOYBUTTONDOWN)
		{
			if (e.jbutton.button == 0 && HeldG == 0)
			{
				//a green fret
				//slog("hit green button");
				level++;
				if (selected == 0)
					setupLevel(tracknum, 0);
				else if (selected == 1)
					setupWriteLevel(tracknum);
				else if (selected == 2)
					setupLevel(tracknum, 1);
				HeldG = 1;
				/* code goes here */
			}
			else if (e.jbutton.button == 1 && HeldR == 0)
			{
				//b red
				//slog("hit red button");
				level--;
				selected = 0;
				HeldR = 1;
				/* code goes here */
			}
		}
		if (e.type == SDL_JOYBUTTONUP)
		{
			HeldG = 0;
			HeldR = 0;
		}
		break;
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
	Sprite *Ricardo = NULL;
/*
	itoa(score, buf, 10);
	text = concat("score: ", buf);
*/
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
	Ricardo = gf2d_sprite_load_all(
		"images/backgrounds/grey ricardo.png",
		600,
		338,
		10,
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
	Shape lineG = gf2d_shape_edge(500,-1,500, 1200);
	Shape lineR = gf2d_shape_edge(600, -1, 600, 1200);
	Shape lineB = gf2d_shape_edge(700, -1, 700, 1200);
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
	trackName = loadTrackNameFromFile("TrackName");

	while (SDL_PollEvent(&e) != 0 || !_done)
	{
		
		gf2d_input_update();
		/*update things here*/
		gf2d_windows_update_all();
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

		//gf2d_sprite_draw(Ricardo, vector2d(600, 350), scale, scaleC, NULL, NULL, &colorS, (frame / 4) % 201);
		//if (frame > 804)
		//{
		//	vector4d_set(colorS, (float)rand() / (float)255, (float)rand() / (float)255, (float)rand() / (float)255, 255);
		//	frame = 0;
		//}
		//frame++;
		// DRAW WORLD
		Menu();

		
		//mode play(1) or write(2)
		switch (mode)
		{
		case 1:
			gf2d_shape_draw(lineG, gf2d_color(0, 255, 0, 255), vector2d(0, 0));
			gf2d_shape_draw(lineR, gf2d_color(255, 0, 0, 255), vector2d(0, 0));
			gf2d_shape_draw(lineB, gf2d_color(0, 0, 255, 255), vector2d(0, 0));
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
							//slog("one");
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
							//slog("two");
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
				//scratch
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
				if (e.jbutton.button == 0 && HeldG == 0 && currentlane != 0)
				{
					//a green fret
					slog("hit green button");

					hitNote(trackG, (Entity*)gf2d_list_get_nth(player, 0));
					HeldG = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 1 && HeldR == 0)
				{
					//b red
					slog("hit red button");
					hitNote(trackR, (Entity*)gf2d_list_get_nth(player, 1));
					HeldR = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 2 && HeldB == 0 && currentlane != 1)
				{
					//x blue
					slog("hit blue button");
					hitNote(trackB, (Entity*)gf2d_list_get_nth(player, 2));
					HeldB = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 6)
				{
					musicFinishedExit();
					break;
					//back
					
				}
			}
			if (e.type == SDL_JOYBUTTONUP)
			{
				HeldG = 0;
				HeldB = 0;
				HeldR = 0;
			}
			//draw Score
			gf2d_text_draw_line(text, FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 0));
			gf2d_text_draw_line(streakT, FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 50));
			gf2d_text_draw_line(multi, FT_H1, gf2d_color(255, 255, 255, 255), vector2d(0, 100));
			gf2d_particle_emitter_update(pe);
			gf2d_particle_emitter_draw(pe);
			Mix_HookMusicFinished(musicFinished);
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
					}/*
					if (e.jaxis.axis == 1)
					{
						if (e.jaxis.value > 6 && HeldG && scratchdown)
						{
							slog("down G");
							scratch_new(((Entity*)gf2d_list_get_nth(player, 0))->position, vector4d(0, 255, 0, 255));
							scratchup = 1;
							scratchdown = 0;
						}
						else if (e.jaxis.value < -6 && HeldG && scratchup)
						{
							slog("up G");
							scratch_new(((Entity*)gf2d_list_get_nth(player, 0))->position, vector4d(0, 255, 0, 255));
							scratchup = 0;
							scratchdown = 1;
						}
						if (e.jaxis.value > 6 && HeldR && scratchdown)
						{
							slog("down R");
							scratch_new(((Entity*)gf2d_list_get_nth(player, 1))->position, vector4d(255, 0, 0, 255));
							scratchup = 1;
							scratchdown = 0;
						}
						else if (e.jaxis.value < -6 && HeldR && scratchup)
						{
							slog("up R");
							scratch_new(((Entity*)gf2d_list_get_nth(player, 1))->position, vector4d(255, 0, 0, 255));
							scratchup = 0;
							scratchdown = 1;

						}
						if (e.jaxis.value > 6 && HeldB && scratchdown)
						{
							scratch_new(((Entity*)gf2d_list_get_nth(player, 2))->position, vector4d(0, 0, 255, 255));
							slog("down B");
							scratchup = 1;
							scratchdown = 0;
						}
						else if (e.jaxis.value < -6 && HeldB && scratchup)
						{
							scratch_new(((Entity*)gf2d_list_get_nth(player, 2))->position, vector4d(0, 0, 255, 255));
							slog("up B");
							scratchup = 0;
							scratchdown = 1;
						}
					}*/
				}
			}
			else if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == 0 && HeldG == 0 && currentlane != 0)
				{
					//a green fret
					//slog("%f", ((Entity*)gf2d_list_get_nth(player, 0))->position.x);
					track = gf2d_list_append(track, (fret_new(((Entity*)gf2d_list_get_nth(player, 0))->position, vector4d(0, 255, 0, 255))));
					HeldG = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 1 && HeldR == 0)
				{
					//b red
					slog("hit red button");
					track = gf2d_list_append(track, (fret_new(((Entity*)gf2d_list_get_nth(player, 1))->position, vector4d(255, 0, 0, 255))));
					HeldR = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 2 && HeldB == 0 && currentlane != 1)
				{
					//x blue
					slog("hit blue button");
					track = gf2d_list_append(track, (fret_new(((Entity*)gf2d_list_get_nth(player, 2))->position, vector4d(0, 0, 255, 255))));
					HeldB = 1;
					/* code goes here */
				}
				if (e.jbutton.button == 7 && Held == 0)
				{
					//start
					writeTrackToFile(concat((char*)gf2d_list_get_nth(trackName, tracknum), " W"));
					Held = 1;
					break;
				}
				if (e.jbutton.button == 6)
				{
					musicFinishedExit();
					//back
				}
			}
			if (e.jbutton.type == SDL_JOYBUTTONUP)
			{
				HeldG = 0;
				HeldB = 0;
				HeldR = 0;
				Held = 0;
				scratchup = 1;
				scratchdown = 1;
			}
			Mix_HookMusicFinished(musicFinishedWrite);
		default:
			break;
		}

		//line to each note
		gf2d_entity_draw_all();

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
