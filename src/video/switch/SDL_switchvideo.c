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

#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_SWITCH

/* SDL internals */
#include "../SDL_sysvideo.h"
#include "../../events/SDL_keyboard_c.h"

#include <switch.h>

#define SWITCH_DATA "_SDL_SwitchData"
#define SCREEN_WIDTH    1280
#define SCREEN_HEIGHT   720

static int SWITCH_VideoInit(_THIS);

static int SWITCH_SetDisplayMode(_THIS, SDL_VideoDisplay *display, SDL_DisplayMode *mode);

static void SWITCH_VideoQuit(_THIS);

static void SWITCH_PumpEvents(_THIS);

static int SWITCH_CreateWindowFramebuffer(_THIS, SDL_Window *window, Uint32 *format, void **pixels, int *pitch);

static int SWITCH_UpdateWindowFramebuffer(_THIS, SDL_Window *window, const SDL_Rect *rects, int numrects);

static void SWITCH_DestroyWindowFramebuffer(_THIS, SDL_Window *window);

static int SWITCH_Available(void)
{
    return 1;
}

static void SWITCH_DeleteDevice(SDL_VideoDevice *device)
{
    SDL_free(device);
}

static SDL_VideoDevice *SWITCH_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return NULL;
    }

    device->VideoInit = SWITCH_VideoInit;
    device->VideoQuit = SWITCH_VideoQuit;
    device->SetDisplayMode = SWITCH_SetDisplayMode;
    device->PumpEvents = SWITCH_PumpEvents;
    device->CreateWindowFramebuffer = SWITCH_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = SWITCH_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = SWITCH_DestroyWindowFramebuffer;

    device->free = SWITCH_DeleteDevice;

    return device;
}

VideoBootStrap SWITCH_bootstrap = {
    "Switch", "Video driver for Nintendo Switch (libnx)",
    SWITCH_Available, SWITCH_CreateDevice
};

static int SWITCH_VideoInit(_THIS)
{
    SDL_DisplayMode mode;

    consoleDebugInit(debugDevice_SVC);
    stdout = stderr;

    gfxInitResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfxInitDefault();
    gfxSetMode(GfxMode_LinearDouble);

    mode.format = SDL_PIXELFORMAT_ARGB8888;
    mode.w = SCREEN_WIDTH;
    mode.h = SCREEN_HEIGHT;
    mode.refresh_rate = 60;
    mode.driverdata = NULL;
    if (SDL_AddBasicVideoDisplay(&mode) < 0) {
        return -1;
    }

    SDL_AddDisplayMode(&_this->displays[0], &mode);

    return 0;
}

static void SWITCH_VideoQuit(_THIS)
{
    gfxExit();
}

static int SWITCH_SetDisplayMode(_THIS, SDL_VideoDisplay *display, SDL_DisplayMode *mode)
{
    // TODO: use gfxConfigureResolution
    return 0;
}

static void SWITCH_PumpEvents(_THIS)
{
    if (!appletMainLoop()) {
        SDL_Event ev;
        ev.type = SDL_QUIT;
        SDL_PushEvent(&ev);
        return;
    }

    hidScanInput();
}

typedef struct
{
} SWITCH_WindowData;

static int SWITCH_CreateWindowFramebuffer(_THIS, SDL_Window *window, Uint32 *format, void **pixels, int *pitch)
{
    int w, h;

    //SWITCH_WindowData *data = SDL_calloc(1, sizeof(SWITCH_WindowData));
    //SDL_SetWindowData(window, SWITCH_DATA, data);

    SDL_GetWindowSize(window, &w, &h);
    gfxConfigureResolution(w, h);
    *pitch = w * 4;
    *format = SDL_PIXELFORMAT_ABGR8888;
    *pixels = gfxGetFramebuffer(NULL, NULL);

    SDL_SetKeyboardFocus(window);

    return 0;
}

static int SWITCH_UpdateWindowFramebuffer(_THIS, SDL_Window *window, const SDL_Rect *rects, int numrects)
{
    //SWITCH_WindowData *data = (SWITCH_WindowData *) SDL_GetWindowData(window, SWITCH_DATA);
    //if (!data) {
    //    return SDL_SetError("Couldn't find switch data for window");
    //}

    gfxFlushBuffers();
    gfxSwapBuffers();
    gfxWaitForVsync();

    // update fb ptr for double buffering
    window->surface->pixels = gfxGetFramebuffer(NULL, NULL);

    return 0;
}

static void SWITCH_DestroyWindowFramebuffer(_THIS, SDL_Window *window)
{
    //SWITCH_WindowData *data = (SWITCH_WindowData *) SDL_GetWindowData(window, SWITCH_DATA);
    //SDL_free(data);
}

#endif /* SDL_VIDEO_DRIVER_SWITCH */