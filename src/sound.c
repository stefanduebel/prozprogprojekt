#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

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
}

void playMenuMusic()
{
	Mix_Music *music;
	music=Mix_LoadMUS("resources/sounds/wasteland_looped_no_choir.ogg");
	if(!music) {
		 printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
		 // this might be a critical error...
	}
	Mix_FadeInMusic(music, -1, 500);
}

void stopMusic()
{
	Mix_FadeOutMusic(500);
}

void playGameMusic()
{
	Mix_Music *music;
	music=Mix_LoadMUS("resources/sounds/inis_mona.ogg");
	if(!music) {
		 printf("Mix_LoadMUS(\"music.mp3\"): %s\n", Mix_GetError());
		 // this might be a critical error...
	}
	Mix_FadeInMusic(music, -1, 500);
}

void playCoinSound(char num)
{
	printf("play coin sound\n");
	Mix_Chunk *sound;
	sound = Mix_LoadWAV("resources/sounds/8_bit_coins/coin1.ogg");
	if(sound == NULL) {
		fprintf(stderr, "Unable to load WAV file: %s\n", Mix_GetError());
	}

	int channel;

	 channel = Mix_PlayChannel(-1, sound, 0);
	 if(channel == -1) {
		 fprintf(stderr, "Unable to play WAV file: %s\n", Mix_GetError());
	 }
}
