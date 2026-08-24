#include "input.h"

#include <string.h>

static unsigned int buttons;
static unsigned int previous_buttons;
static unsigned int pressed_frame;
static unsigned int released_frame;
static unsigned int pressed_accum;
static unsigned int released_accum;
static unsigned int lock_mask;

void input_init(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);
}

void input_update(void) {
    SceCtrlData controller;
    memset(&controller, 0, sizeof(controller));
    if (sceCtrlPeekBufferPositive(0, &controller, 1) <= 0) return;

    buttons = controller.buttons;
    if (controller.lx < 72) buttons |= PSP_CTRL_LEFT;
    if (controller.lx > 184) buttons |= PSP_CTRL_RIGHT;
    if (controller.ly < 72) buttons |= PSP_CTRL_UP;
    if (controller.ly > 184) buttons |= PSP_CTRL_DOWN;

    pressed_frame = buttons & ~previous_buttons;
    released_frame = ~buttons & previous_buttons;
    pressed_accum |= pressed_frame;
    released_accum |= released_frame;
    previous_buttons = buttons;
    lock_mask &= ~released_frame;
}

bool input_pressed(unsigned int button) { return (pressed_frame & button & ~lock_mask) != 0; }
bool input_held(unsigned int button) { return (buttons & button & ~lock_mask) != 0; }
bool input_released(unsigned int button) { return (released_frame & button) != 0; }

bool input_consume_pressed(unsigned int button) {
    unsigned int masked = pressed_accum & ~lock_mask;
    if (!(masked & button)) return false;
    pressed_accum &= ~button;
    return true;
}

bool input_consume_released(unsigned int button) {
    if (!(released_accum & button)) return false;
    released_accum &= ~button;
    return true;
}

void input_reset_edges(void) {
    pressed_accum = released_accum = pressed_frame = released_frame = 0;
    previous_buttons = buttons;
}

void input_lock_held(void) { lock_mask |= buttons; }
