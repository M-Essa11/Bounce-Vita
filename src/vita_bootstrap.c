#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/kernel/threadmgr.h>
#include <vita2d.h>

#define SCREEN_WIDTH 960

int main(void) {
    SceCtrlData controller;
    unsigned int previous_buttons = 0;
    int running = 1;
    int accent_enabled = 0;

    vita2d_init();
    vita2d_set_clear_color(RGBA8(13, 19, 31, 255));
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    while (running) {
        unsigned int pressed;

        sceCtrlPeekBufferPositive(0, &controller, 1);
        pressed = controller.buttons & ~previous_buttons;
        previous_buttons = controller.buttons;

        if (pressed & SCE_CTRL_CROSS) accent_enabled = !accent_enabled;
        if (pressed & SCE_CTRL_CIRCLE) running = 0;

        vita2d_start_drawing();
        vita2d_clear_screen();
        vita2d_draw_rectangle(0, 0, SCREEN_WIDTH, 12, RGBA8(44, 194, 255, 255));
        vita2d_draw_rectangle(260, 92, 440, 360, RGBA8(26, 42, 66, 255));
        vita2d_draw_fill_circle(480, 272, 116,
            accent_enabled ? RGBA8(244, 73, 78, 255) : RGBA8(44, 194, 255, 255));
        vita2d_draw_fill_circle(442, 234, 28, RGBA8(255, 255, 255, 255));
        vita2d_draw_fill_circle(518, 234, 28, RGBA8(255, 255, 255, 255));
        vita2d_draw_fill_circle(442, 234, 11, RGBA8(13, 19, 31, 255));
        vita2d_draw_fill_circle(518, 234, 11, RGBA8(13, 19, 31, 255));
        vita2d_end_drawing();
        vita2d_swap_buffers();
        vita2d_wait_rendering_done();
        sceKernelDelayThread(16666);
    }

    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}
