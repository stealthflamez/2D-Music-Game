#ifndef __GAME_H___
#define __GAME_H___

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
#include <stdlib.h>
#include <string.h>
#include "gf2d_particles.h"
#include "gf2d_types.h"

#include "player.h"
#include "fret.h"
/**
 * @brief Creates particals when player hits notes
 */
void hitNoteFX();
/**
 * @brief checks if player and note care colliding
 * @param List of notes
 * @param pointer to player
 */
void hitNote(List *track, Entity *player);
/**
 * @brief Writes position of frets to filename
 * @param file to save to
 * @param List of frets
 */
void writeTrackToFile(char *filename, List *track);
/**
 * @brief At the end of song writes highscore to file
 */
void SaveHighScore();
/**
 * @brief Allows ZXC to create frets and adds to track
 * @param List to where frets will be saved to
 */
void writeTrack(List *track);
/**
 * @brief loads from filename position of every fret in filename
 * @param file to load from
 * @return NULL on a List of frets with position
 */
List *loadTrackFromFile(char *filename);
/**
 * @brief setup player, frets, music
 * @param tracknum controls which track to load
 */
void setupLevel(int tracknum);
/**
 * @brief setup player, frets to write to, music
 * @param tracknum controls which track to load
 */
void setupWriteLevel(int tracknum);
/**
 * @brief Control Menu navigation
 */
void Menu();
#endif