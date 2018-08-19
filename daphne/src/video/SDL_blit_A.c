/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include <stdint.h>
#include "../SDL_internal.h"

#include "SDL_video.h"
#include "SDL_blit.h"

/* Functions to perform alpha blended blitting */

/* N->1 blending with per-surface alpha */
static void
BlitNto1SurfaceAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    uint8_t *palmap = info->table;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    int srcbpp = srcfmt->BytesPerPixel;
    uint32_t Pixel;
    unsigned sR, sG, sB;
    unsigned dR, dG, dB;
    const unsigned A = info->a;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4(
	    {
		DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
		dR = dstfmt->palette->colors[*dst].r;
		dG = dstfmt->palette->colors[*dst].g;
		dB = dstfmt->palette->colors[*dst].b;
		ALPHA_BLEND_RGB(sR, sG, sB, A, dR, dG, dB);
		dR &= 0xff;
		dG &= 0xff;
		dB &= 0xff;
		/* Pack RGB into 8bit pixel */
		if ( palmap == NULL ) {
		    *dst =((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0));
		} else {
		    *dst = palmap[((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0))];
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    /* *INDENT-ON* */
        src += srcskip;
        dst += dstskip;
    }
}

/* N->1 blending with pixel alpha */
static void
BlitNto1PixelAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    uint8_t *palmap = info->table;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    int srcbpp = srcfmt->BytesPerPixel;
    uint32_t Pixel;
    unsigned sR, sG, sB, sA;
    unsigned dR, dG, dB;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4(
	    {
		DISEMBLE_RGBA(src,srcbpp,srcfmt,Pixel,sR,sG,sB,sA);
		dR = dstfmt->palette->colors[*dst].r;
		dG = dstfmt->palette->colors[*dst].g;
		dB = dstfmt->palette->colors[*dst].b;
		ALPHA_BLEND_RGB(sR, sG, sB, sA, dR, dG, dB);
		dR &= 0xff;
		dG &= 0xff;
		dB &= 0xff;
		/* Pack RGB into 8bit pixel */
		if ( palmap == NULL ) {
		    *dst =((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0));
		} else {
		    *dst = palmap[((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0))];
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    /* *INDENT-ON* */
        src += srcskip;
        dst += dstskip;
    }
}

/* colorkeyed N->1 blending with per-surface alpha */
static void
BlitNto1SurfaceAlphaKey(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    uint8_t *palmap = info->table;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    int srcbpp = srcfmt->BytesPerPixel;
    uint32_t ckey = info->colorkey;
    uint32_t Pixel;
    unsigned sR, sG, sB;
    unsigned dR, dG, dB;
    const unsigned A = info->a;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP(
	    {
		DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
		if ( Pixel != ckey ) {
		    dR = dstfmt->palette->colors[*dst].r;
		    dG = dstfmt->palette->colors[*dst].g;
		    dB = dstfmt->palette->colors[*dst].b;
		    ALPHA_BLEND_RGB(sR, sG, sB, A, dR, dG, dB);
		    dR &= 0xff;
		    dG &= 0xff;
		    dB &= 0xff;
		    /* Pack RGB into 8bit pixel */
		    if ( palmap == NULL ) {
                *dst =((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0));
		    } else {
                *dst = palmap[((dR>>5)<<(3+2))|((dG>>5)<<(2))|((dB>>6)<<(0))];
		    }
		}
		dst++;
		src += srcbpp;
	    },
	    width);
	    /* *INDENT-ON* */
        src += srcskip;
        dst += dstskip;
    }
}

/* fast RGB888->(A)RGB888 blending with surface alpha=128 special case */
static void
BlitRGBtoRGBSurfaceAlpha128(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint32_t *srcp = (uint32_t *) info->src;
    int srcskip = info->src_skip >> 2;
    uint32_t *dstp = (uint32_t *) info->dst;
    int dstskip = info->dst_skip >> 2;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4({
		    uint32_t s = *srcp++;
		    uint32_t d = *dstp;
		    *dstp++ = ((((s & 0x00fefefe) + (d & 0x00fefefe)) >> 1)
			       + (s & d & 0x00010101)) | 0xff000000;
	    }, width);
	    /* *INDENT-ON* */
        srcp += srcskip;
        dstp += dstskip;
    }
}

/* fast RGB888->(A)RGB888 blending with surface alpha */
static void
BlitRGBtoRGBSurfaceAlpha(SDL_BlitInfo * info)
{
    unsigned alpha = info->a;
    if (alpha == 128) {
        BlitRGBtoRGBSurfaceAlpha128(info);
    } else {
        int width = info->dst_w;
        int height = info->dst_h;
        uint32_t *srcp = (uint32_t *) info->src;
        int srcskip = info->src_skip >> 2;
        uint32_t *dstp = (uint32_t *) info->dst;
        int dstskip = info->dst_skip >> 2;
        uint32_t s;
        uint32_t d;
        uint32_t s1;
        uint32_t d1;

        while (height--) {
			/* *INDENT-OFF* */
			DUFFS_LOOP4({
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 = (d1 + ((s1 - d1) * alpha >> 8))
				     & 0xff00ff;
				s &= 0xff00;
				d &= 0xff00;
				d = (d + ((s - d) * alpha >> 8)) & 0xff00;
				*dstp = d1 | d | 0xff000000;
				++srcp;
				++dstp;
			}, width);
			/* *INDENT-ON* */
            srcp += srcskip;
            dstp += dstskip;
        }
    }
}

/* fast ARGB888->(A)RGB888 blending with pixel alpha */
static void
BlitRGBtoRGBPixelAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint32_t *srcp = (uint32_t *) info->src;
    int srcskip = info->src_skip >> 2;
    uint32_t *dstp = (uint32_t *) info->dst;
    int dstskip = info->dst_skip >> 2;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4({
		uint32_t dalpha;
		uint32_t d;
		uint32_t s1;
		uint32_t d1;
		uint32_t s = *srcp;
		uint32_t alpha = s >> 24;
		/* FIXME: Here we special-case opaque alpha since the
		   compositioning used (>>8 instead of /255) doesn't handle
		   it correctly. Also special-case alpha=0 for speed?
		   Benchmark this! */
		if (alpha) {
		  if (alpha == SDL_ALPHA_OPAQUE) {
			  *dstp = *srcp;
		  } else {
		    /*
		     * take out the middle component (green), and process
		     * the other two in parallel. One multiply less.
		     */
		    d = *dstp;
			dalpha = d >> 24;
		    s1 = s & 0xff00ff;
		    d1 = d & 0xff00ff;
		    d1 = (d1 + ((s1 - d1) * alpha >> 8)) & 0xff00ff;
		    s &= 0xff00;
		    d &= 0xff00;
		    d = (d + ((s - d) * alpha >> 8)) & 0xff00;
			dalpha = alpha + (dalpha * (alpha ^ 0xFF) >> 8);
		    *dstp = d1 | d | (dalpha << 24);
		  }
		}
		++srcp;
		++dstp;
	    }, width);
	    /* *INDENT-ON* */
        srcp += srcskip;
        dstp += dstskip;
    }
}

/* 16bpp special case for per-surface alpha=50%: blend 2 pixels in parallel */

/* blend a single 16 bit pixel at 50% */
#define BLEND16_50(d, s, mask)						\
	((((s & mask) + (d & mask)) >> 1) + (s & d & (~mask & 0xffff)))

/* blend two 16 bit pixels at 50% */
#define BLEND2x16_50(d, s, mask)					     \
	(((s & (mask | mask << 16)) >> 1) + ((d & (mask | mask << 16)) >> 1) \
	 + (s & d & (~(mask | mask << 16))))

static void
Blit16to16SurfaceAlpha128(SDL_BlitInfo * info, uint16_t mask)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint16_t *srcp = (uint16_t *) info->src;
    int srcskip = info->src_skip >> 1;
    uint16_t *dstp = (uint16_t *) info->dst;
    int dstskip = info->dst_skip >> 1;

    while (height--) {
        if (((uintptr_t) srcp ^ (uintptr_t) dstp) & 2) {
            /*
             * Source and destination not aligned, pipeline it.
             * This is mostly a win for big blits but no loss for
             * small ones
             */
            uint32_t prev_sw;
            int w = width;

            /* handle odd destination */
            if ((uintptr_t) dstp & 2) {
                uint16_t d = *dstp, s = *srcp;
                *dstp = BLEND16_50(d, s, mask);
                dstp++;
                srcp++;
                w--;
            }
            srcp++;             /* srcp is now 32-bit aligned */

            /* bootstrap pipeline with first halfword */
            prev_sw = ((uint32_t *) srcp)[-1];

            while (w > 1) {
                uint32_t sw, dw, s;
                sw = *(uint32_t *) srcp;
                dw = *(uint32_t *) dstp;
#ifdef MSB_FIRST
                s = (prev_sw << 16) + (sw >> 16);
#else
                s = (prev_sw >> 16) + (sw << 16);
#endif
                prev_sw = sw;
                *(uint32_t *) dstp = BLEND2x16_50(dw, s, mask);
                dstp += 2;
                srcp += 2;
                w -= 2;
            }

            /* final pixel if any */
            if (w) {
                uint16_t d = *dstp, s;
#ifdef MSB_FIRST
                s = (uint16_t) prev_sw;
#else
                s = (uint16_t) (prev_sw >> 16);
#endif
                *dstp = BLEND16_50(d, s, mask);
                srcp++;
                dstp++;
            }
            srcp += srcskip - 1;
            dstp += dstskip;
        } else {
            /* source and destination are aligned */
            int w = width;

            /* first odd pixel? */
            if ((uintptr_t) srcp & 2) {
                uint16_t d = *dstp, s = *srcp;
                *dstp = BLEND16_50(d, s, mask);
                srcp++;
                dstp++;
                w--;
            }
            /* srcp and dstp are now 32-bit aligned */

            while (w > 1) {
                uint32_t sw = *(uint32_t *) srcp;
                uint32_t dw = *(uint32_t *) dstp;
                *(uint32_t *) dstp = BLEND2x16_50(dw, sw, mask);
                srcp += 2;
                dstp += 2;
                w -= 2;
            }

            /* last odd pixel? */
            if (w) {
                uint16_t d = *dstp, s = *srcp;
                *dstp = BLEND16_50(d, s, mask);
                srcp++;
                dstp++;
            }
            srcp += srcskip;
            dstp += dstskip;
        }
    }
}

/* fast RGB565->RGB565 blending with surface alpha */
static void
Blit565to565SurfaceAlpha(SDL_BlitInfo * info)
{
    unsigned alpha = info->a;
    if (alpha == 128) {
        Blit16to16SurfaceAlpha128(info, 0xf7de);
    } else {
        int width = info->dst_w;
        int height = info->dst_h;
        uint16_t *srcp = (uint16_t *) info->src;
        int srcskip = info->src_skip >> 1;
        uint16_t *dstp = (uint16_t *) info->dst;
        int dstskip = info->dst_skip >> 1;
        alpha >>= 3;            /* downscale alpha to 5 bits */

        while (height--) {
			/* *INDENT-OFF* */
			DUFFS_LOOP4({
				uint32_t s = *srcp++;
				uint32_t d = *dstp;
				/*
				 * shift out the middle component (green) to
				 * the high 16 bits, and process all three RGB
				 * components at the same time.
				 */
				s = (s | s << 16) & 0x07e0f81f;
				d = (d | d << 16) & 0x07e0f81f;
				d += (s - d) * alpha >> 5;
				d &= 0x07e0f81f;
				*dstp++ = (uint16_t)(d | d >> 16);
			}, width);
			/* *INDENT-ON* */
            srcp += srcskip;
            dstp += dstskip;
        }
    }
}

/* fast RGB555->RGB555 blending with surface alpha */
static void
Blit555to555SurfaceAlpha(SDL_BlitInfo * info)
{
    unsigned alpha = info->a;   /* downscale alpha to 5 bits */
    if (alpha == 128) {
        Blit16to16SurfaceAlpha128(info, 0xfbde);
    } else {
        int width = info->dst_w;
        int height = info->dst_h;
        uint16_t *srcp = (uint16_t *) info->src;
        int srcskip = info->src_skip >> 1;
        uint16_t *dstp = (uint16_t *) info->dst;
        int dstskip = info->dst_skip >> 1;
        alpha >>= 3;            /* downscale alpha to 5 bits */

        while (height--) {
			/* *INDENT-OFF* */
			DUFFS_LOOP4({
				uint32_t s = *srcp++;
				uint32_t d = *dstp;
				/*
				 * shift out the middle component (green) to
				 * the high 16 bits, and process all three RGB
				 * components at the same time.
				 */
				s = (s | s << 16) & 0x03e07c1f;
				d = (d | d << 16) & 0x03e07c1f;
				d += (s - d) * alpha >> 5;
				d &= 0x03e07c1f;
				*dstp++ = (uint16_t)(d | d >> 16);
			}, width);
			/* *INDENT-ON* */
            srcp += srcskip;
            dstp += dstskip;
        }
    }
}

/* fast ARGB8888->RGB565 blending with pixel alpha */
static void
BlitARGBto565PixelAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint32_t *srcp = (uint32_t *) info->src;
    int srcskip = info->src_skip >> 2;
    uint16_t *dstp = (uint16_t *) info->dst;
    int dstskip = info->dst_skip >> 1;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4({
		uint32_t s = *srcp;
		unsigned alpha = s >> 27; /* downscale alpha to 5 bits */
		/* FIXME: Here we special-case opaque alpha since the
		   compositioning used (>>8 instead of /255) doesn't handle
		   it correctly. Also special-case alpha=0 for speed?
		   Benchmark this! */
		if(alpha) {   
		  if(alpha == (SDL_ALPHA_OPAQUE >> 3)) {
		    *dstp = (uint16_t)((s >> 8 & 0xf800) + (s >> 5 & 0x7e0) + (s >> 3  & 0x1f));
		  } else {
		    uint32_t d = *dstp;
		    /*
		     * convert source and destination to G0RAB65565
		     * and blend all components at the same time
		     */
		    s = ((s & 0xfc00) << 11) + (s >> 8 & 0xf800)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x07e0f81f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x07e0f81f;
		    *dstp = (uint16_t)(d | d >> 16);
		  }
		}
		srcp++;
		dstp++;
	    }, width);
	    /* *INDENT-ON* */
        srcp += srcskip;
        dstp += dstskip;
    }
}

/* fast ARGB8888->RGB555 blending with pixel alpha */
static void
BlitARGBto555PixelAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint32_t *srcp = (uint32_t *) info->src;
    int srcskip = info->src_skip >> 2;
    uint16_t *dstp = (uint16_t *) info->dst;
    int dstskip = info->dst_skip >> 1;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4({
		unsigned alpha;
		uint32_t s = *srcp;
		alpha = s >> 27; /* downscale alpha to 5 bits */
		/* FIXME: Here we special-case opaque alpha since the
		   compositioning used (>>8 instead of /255) doesn't handle
		   it correctly. Also special-case alpha=0 for speed?
		   Benchmark this! */
		if(alpha) {   
		  if(alpha == (SDL_ALPHA_OPAQUE >> 3)) {
		    *dstp = (uint16_t)((s >> 9 & 0x7c00) + (s >> 6 & 0x3e0) + (s >> 3  & 0x1f));
		  } else {
		    uint32_t d = *dstp;
		    /*
		     * convert source and destination to G0RAB65565
		     * and blend all components at the same time
		     */
		    s = ((s & 0xf800) << 10) + (s >> 9 & 0x7c00)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x03e07c1f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x03e07c1f;
		    *dstp = (uint16_t)(d | d >> 16);
		  }
		}
		srcp++;
		dstp++;
	    }, width);
	    /* *INDENT-ON* */
        srcp += srcskip;
        dstp += dstskip;
    }
}

/* General (slow) N->N blending with per-surface alpha */
static void
BlitNtoNSurfaceAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    int srcbpp = srcfmt->BytesPerPixel;
    int dstbpp = dstfmt->BytesPerPixel;
    uint32_t Pixel;
    unsigned sR, sG, sB;
    unsigned dR, dG, dB, dA;
    const unsigned sA = info->a;

    if (sA) {
        while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4(
	    {
		DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
		DISEMBLE_RGBA(dst, dstbpp, dstfmt, Pixel, dR, dG, dB, dA);
		ALPHA_BLEND_RGBA(sR, sG, sB, sA, dR, dG, dB, dA);
		ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    /* *INDENT-ON* */
            src += srcskip;
            dst += dstskip;
        }
    }
}

/* General (slow) colorkeyed N->N blending with per-surface alpha */
static void
BlitNtoNSurfaceAlphaKey(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    uint32_t ckey = info->colorkey;
    int srcbpp = srcfmt->BytesPerPixel;
    int dstbpp = dstfmt->BytesPerPixel;
    uint32_t Pixel;
    unsigned sR, sG, sB;
    unsigned dR, dG, dB, dA;
    const unsigned sA = info->a;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4(
	    {
		RETRIEVE_RGB_PIXEL(src, srcbpp, Pixel);
		if(sA && Pixel != ckey) {
		    RGB_FROM_PIXEL(Pixel, srcfmt, sR, sG, sB);
		    DISEMBLE_RGBA(dst, dstbpp, dstfmt, Pixel, dR, dG, dB, dA);
		    ALPHA_BLEND_RGBA(sR, sG, sB, sA, dR, dG, dB, dA);
		    ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		}
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    /* *INDENT-ON* */
        src += srcskip;
        dst += dstskip;
    }
}

/* General (slow) N->N blending with pixel alpha */
static void
BlitNtoNPixelAlpha(SDL_BlitInfo * info)
{
    int width = info->dst_w;
    int height = info->dst_h;
    uint8_t *src = info->src;
    int srcskip = info->src_skip;
    uint8_t *dst = info->dst;
    int dstskip = info->dst_skip;
    SDL_PixelFormat *srcfmt = info->src_fmt;
    SDL_PixelFormat *dstfmt = info->dst_fmt;
    int srcbpp;
    int dstbpp;
    uint32_t Pixel;
    unsigned sR, sG, sB, sA;
    unsigned dR, dG, dB, dA;

    /* Set up some basic variables */
    srcbpp = srcfmt->BytesPerPixel;
    dstbpp = dstfmt->BytesPerPixel;

    while (height--) {
	    /* *INDENT-OFF* */
	    DUFFS_LOOP4(
	    {
		DISEMBLE_RGBA(src, srcbpp, srcfmt, Pixel, sR, sG, sB, sA);
		if(sA) {
		    DISEMBLE_RGBA(dst, dstbpp, dstfmt, Pixel, dR, dG, dB, dA);
		    ALPHA_BLEND_RGBA(sR, sG, sB, sA, dR, dG, dB, dA);
		    ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		}
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    /* *INDENT-ON* */
        src += srcskip;
        dst += dstskip;
    }
}


SDL_BlitFunc
SDL_CalculateBlitA(SDL_Surface * surface)
{
    SDL_PixelFormat *sf = surface->format;
    SDL_PixelFormat *df = surface->map->dst->format;

    switch (surface->map->info.flags & ~SDL_COPY_RLE_MASK) {
    case SDL_COPY_BLEND:
        /* Per-pixel alpha blits */
        switch (df->BytesPerPixel) {
        case 1:
            return BlitNto1PixelAlpha;

        case 2:
                if (sf->BytesPerPixel == 4 && sf->Amask == 0xff000000
                    && sf->Gmask == 0xff00
                    && ((sf->Rmask == 0xff && df->Rmask == 0x1f)
                        || (sf->Bmask == 0xff && df->Bmask == 0x1f))) {
                if (df->Gmask == 0x7e0)
                    return BlitARGBto565PixelAlpha;
                else if (df->Gmask == 0x3e0)
                    return BlitARGBto555PixelAlpha;
            }
            return BlitNtoNPixelAlpha;

        case 4:
            if (sf->Rmask == df->Rmask
                && sf->Gmask == df->Gmask
                && sf->Bmask == df->Bmask && sf->BytesPerPixel == 4) {
                if (sf->Amask == 0xff000000) {
                    return BlitRGBtoRGBPixelAlpha;
                }
            }
            return BlitNtoNPixelAlpha;

        case 3:
        default:
            return BlitNtoNPixelAlpha;
        }
        break;

    case SDL_COPY_MODULATE_ALPHA | SDL_COPY_BLEND:
        if (sf->Amask == 0) {
            /* Per-surface alpha blits */
            switch (df->BytesPerPixel) {
            case 1:
                return BlitNto1SurfaceAlpha;

            case 2:
                if (surface->map->identity) {
                    if (df->Gmask == 0x7e0) {
                            return Blit565to565SurfaceAlpha;
                    } else if (df->Gmask == 0x3e0) {
                            return Blit555to555SurfaceAlpha;
                    }
                }
                return BlitNtoNSurfaceAlpha;

            case 4:
                if (sf->Rmask == df->Rmask
                    && sf->Gmask == df->Gmask
                    && sf->Bmask == df->Bmask && sf->BytesPerPixel == 4) {
                    if ((sf->Rmask | sf->Gmask | sf->Bmask) == 0xffffff) {
                        return BlitRGBtoRGBSurfaceAlpha;
                    }
                }
                return BlitNtoNSurfaceAlpha;

            case 3:
            default:
                return BlitNtoNSurfaceAlpha;
            }
        }
        break;

    case SDL_COPY_COLORKEY | SDL_COPY_MODULATE_ALPHA | SDL_COPY_BLEND:
        if (sf->Amask == 0) {
            if (df->BytesPerPixel == 1) {
                return BlitNto1SurfaceAlphaKey;
            } else {
                return BlitNtoNSurfaceAlphaKey;
            }
        }
        break;
    }

    return NULL;
}

/* vi: set ts=4 sw=4 expandtab: */
