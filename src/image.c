#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include "image.h"

typedef struct tColorRGBA {
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;
} tColorRGBA;

SDL_Surface *shrinkSurface(SDL_Surface * src, int w, int h)
{
	printf("resizing image --- ");
	// create new Surface
	SDL_Surface *dst = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

	// compute shrink factors
	int factorx, factory;
	factorx = src->clip_rect.w / w;
	factory = src->clip_rect.h / h;
	int x, y, dx, dy, dgap, ra, ga, ba, aa;
	int n_average;
	tColorRGBA *sp, *osp, *oosp;
	tColorRGBA *dp;

	/*
	* Averaging integer shrink
	*/

	/* Precalculate division factor */
	n_average = factorx*factory;

	/*
	* Scan destination
	*/
	sp = (tColorRGBA *) src->pixels;
	
	dp = (tColorRGBA *) dst->pixels;
	dgap = dst->pitch - dst->w * 4;

	// Gehe durch alle Pixel im Zielbild
	for (y = 0; y < dst->h; y++) {

		osp=sp;
		for (x = 0; x < dst->w; x++) {

			/* Trace out source box and accumulate */
			oosp=sp;
			ra=ga=ba=aa=0;

			for (dy=0; dy < factory; dy++) {
				for (dx=0; dx < factorx; dx++) {
					ra += sp->r;
					ga += sp->g;
					ba += sp->b;
					aa += sp->a;

					sp++;
				} 
				/* src dx loop */
				sp = (tColorRGBA *)((Uint8*)sp + (src->pitch - 4*factorx)); // next y
			}
			/* src dy loop */

			/* next box-x */
			sp = (tColorRGBA *)((Uint8*)oosp + 4*factorx);

			/* Store result in destination */
			dp->r = ra/n_average;
			dp->g = ga/n_average;
			dp->b = ba/n_average;
			dp->a = aa/n_average;

			/*
			* Advance destination pointer 
			*/
			dp++;
		} 
		/* dst x loop */

		/* next box-y */
		sp = (tColorRGBA *)((Uint8*)osp + src->pitch*factory);

		/*
		* Advance destination pointers 
		*/
		dp = (tColorRGBA *) ((Uint8 *) dp + dgap);
	} 
	/* dst y loop */

	printf("finished\n");
	return dst;
}
