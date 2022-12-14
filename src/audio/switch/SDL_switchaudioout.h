/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2018 Sam Lantinga <slouken@libsdl.org>

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

#ifndef SDL_switchaudio_h_
#define SDL_switchaudio_h_

#include <switch.h>
#include "../SDL_sysaudio.h"

/* Hidden "this" pointer for the audio functions */
#define _THIS   SDL_AudioDevice *this

#define NUM_BUFFERS 2

struct SDL_PrivateAudioData
{
    AudioOutBuffer buffer[NUM_BUFFERS];
    AudioOutBuffer *released_out_buffer;
    u32 released_out_count;
    /* The raw allocated mixing buffer. */
    Uint8   *rawbuf;
    /* Individual mixing buffers. */
    void *out_buffers[NUM_BUFFERS];
    /* Index of the next available mixing buffer. */
    int     next_buffer;
    /* Currently playing buffer */
    int     cur_buffer;
};

#endif /* SDL_switchaudio_h_ */
