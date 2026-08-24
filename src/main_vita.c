#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <stdio.h>

#include "game.h"
#include "graphics.h"
#include "input.h"
#include "platform.h"
#include "sound.h"
#include "types.h"

#define PHYSICS_DT_MS 30

int g_vita_audio_status = 0;

FILE *util_open_file(const char *path, const char *mode) {
    char full_path[512];
    FILE *file;
    if (!path || !mode) return NULL;
    file = fopen(path, mode);
    if (file) return file;
    snprintf(full_path, sizeof(full_path), "app0:/%s", path);
    return fopen(full_path, mode);
}

int main(void) {
    unsigned long long previous_time;
    int physics_accumulator = 0;
    int audio_result;
    GameState previous_state;

    graphics_init();
    input_init();
    audio_result = sound_init();
    g_vita_audio_status = audio_result;
    save_init();
    game_init();
    previous_time = sceKernelGetProcessTimeWide() / 1000ULL;
    previous_state = g_game.state;

    while (g_game.state != STATE_EXIT) {
        const game_state_handler_t *handler;
        input_update();
        handler = game_get_state_handler(g_game.state);
        if (!handler) break;

        if (g_game.state != previous_state) {
            const game_state_handler_t *old_handler = game_get_state_handler(previous_state);
            int was_fixed = old_handler && old_handler->tick_mode == GAME_TICK_FIXED;
            int is_fixed = handler->tick_mode == GAME_TICK_FIXED;
            if (was_fixed && !is_fixed) physics_accumulator = 0;
            else if (!was_fixed && is_fixed) {
                previous_time = sceKernelGetProcessTimeWide() / 1000ULL;
                physics_accumulator = PHYSICS_DT_MS;
            }
            if (g_game.state == STATE_MENU && previous_state != STATE_MENU) save_flush();
            input_reset_edges();
            previous_state = g_game.state;
        }

        if (handler->tick_mode == GAME_TICK_VARIABLE) game_state_update();
        else {
            unsigned long long now;
            int delta;
            if (input_pressed(PSP_CTRL_START)) {
                g_game.saved_game_state = SAVED_GAME_IN_PROGRESS;
                save_set_progress(g_game.selected_level, g_game.score, g_game.numLives, 1);
                g_game.state = STATE_MENU;
            }
            now = sceKernelGetProcessTimeWide() / 1000ULL;
            delta = (int)(now - previous_time);
            previous_time = now;
            if (delta > 200) delta = 200;
            physics_accumulator += delta;
            while (physics_accumulator >= PHYSICS_DT_MS) {
                game_state_update();
                physics_accumulator -= PHYSICS_DT_MS;
                handler = game_get_state_handler(g_game.state);
                if (!handler || handler->tick_mode != GAME_TICK_FIXED) break;
            }
        }

        graphics_start_frame();
        graphics_vita_set_scale(g_game.state == STATE_GAME ? 3.0f : 2.0f);
        game_state_render();
        graphics_end_frame();
        sceKernelDelayThread(1000);
    }

    game_shutdown();
    save_shutdown();
    sound_shutdown();
    graphics_shutdown();
    sceKernelExitProcess(0);
    return 0;
}
