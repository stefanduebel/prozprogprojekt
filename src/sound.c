#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include "sound.h"

Mix_Chunk *sound[BLOCKS + ANIMATIONS];

void initializeSounds()
{
	int audioRate = 22050;
	Uint16 audioFormat = AUDIO_S16SYS;
	int audioChannels = 2;
	int audioBuffers = 4096;

	if(Mix_OpenAudio(audioRate, audioFormat, audioChannels, audioBuffers) != 0) {
		fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
		exit(1);
	}

	/* ---- lade Blocksounds ---- */
	for (int i = 0; i < BLOCKS + ANIMATIONS; i++)
	{
		char file[40];
		sprintf(file, "resources/sounds/%d.ogg", i);
		sound[i] = Mix_LoadWAV(file);
	}
}

void startMusic()
{
	Mix_Music *music;
	music=Mix_LoadMUS("resources/sounds/wasteland_looped_no_choir.ogg");
	if(!music)
	{
		printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
	}
	Mix_FadeInMusic(music, -1, 500);
}

void stopMusic()
{
	Mix_FadeOutMusic(500);
}

void playBlockSound(unsigned int num)
{
	if (sound[num] != NULL && num < BLOCKS + ANIMATIONS)
	{
		int channel = Mix_PlayChannel(-1, sound[num], 0);
		if(channel == -1)
		{
			fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
		}
	}
}
