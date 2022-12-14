/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

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

#include <switch.h>

#include "SDL_events.h"
#include "SDL_hints.h"
#include "../../events/SDL_touch_c.h"
#include "../../video/SDL_sysvideo.h"

HidTouchScreenState touchState;
HidTouchScreenState touchStateOld;

void
SWITCH_InitTouch(void) {
    hidInitializeTouchScreen();
    SDL_AddTouch((SDL_TouchID) 0, SDL_TOUCH_DEVICE_DIRECT, "Switch");
    SDL_SetHintWithPriority(SDL_HINT_TOUCH_MOUSE_EVENTS, "0", SDL_HINT_DEFAULT);
}

void
SWITCH_QuitTouch(void) {

}

void
SWITCH_PollTouch(void) {
    SDL_Window *window = SDL_GetFocusWindow();
    if (window == NULL) {
        return;
    }

    memcpy(&touchStateOld, &touchState, sizeof(touchState));

    if (hidGetTouchScreenStates(&touchState, 1)) {
        for (u32 i = 0; i < touchState.count; i++) {
            // Send an initial touch
            SDL_SendTouch(0, touchState.touches[i].finger_id, window, SDL_TRUE,
                          (float) touchState.touches[i].x / 1280.0f,
                          (float) touchState.touches[i].y / 720.0f, 1);
            // Always send the motion
            SDL_SendTouchMotion(0, (SDL_FingerID) i, window,
                                (float) touchState.touches[i].x / 1280.0f,
                                (float) touchState.touches[i].y / 720.0f, 1);
        }

        // some fingers might have been let go
        if (touchStateOld.count > 0) {
            for (int i = 0; i < touchStateOld.count; i++) {
                int finger_up = 1;
                if (touchState.count > 0) {
                    for (int j = 0; j < touchState.count; j++) {
                        if (touchState.touches[j].finger_id == touchStateOld.touches[i].finger_id) {
                            finger_up = 0;
                        }
                    }
                }
                if (finger_up == 1) {
                    // Finger released from screen
                    SDL_SendTouch((SDL_TouchID) 0, (SDL_FingerID) touchStateOld.touches[i].finger_id, window, SDL_FALSE,
                                  (float) touchStateOld.touches[i].x / 1280.0f,
                                  (float) touchStateOld.touches[i].y / 720.0f, 1);
                }
            }
        }
    }
}

#endif /* SDL_VIDEO_DRIVER_SWITCH */

/* vi: set ts=4 sw=4 expandtab: */
