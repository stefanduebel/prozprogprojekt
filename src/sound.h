#define BLOCKS 100
#define ANIMATIONS 3

/**
 * Initialisiert die Sounds
 */
void initializeSounds();

/**
 * Startet die Hintergrundmusik mit FadeIn Effekt
 */
void startMusic();

/**
 * Stopt die Hintergrundmusik mit FadeOut
 */
void stopMusic();

/**
 * spielt den zu dem Block mit dieser Nummer gehörenden Ton ab, wenn dieser existiert
 *
 * @param num	Index des Blockes (Position im Blockset)
 */
void playBlockSound(unsigned int num);

/**
 * gibt den von Soundelementen belegten Speicher frei und beendet SDL_Mixer
 */
void freeAudio();
