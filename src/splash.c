#include "splash.h"
#include "platform.h"
#include "game.h"
#include "graphics.h"
#include "input.h"
#include "level.h"
#include "local_extra.h"
#include "png.h"
#include "types.h"
#include "graphics.h"

// Утилита для центрирования текстур на экране (splash screens)
static void draw_centered_splash(texture_t* tex) {
    if (!tex) return;
    graphics_begin_textured();

    // Используем реальный размер изображения, а не размер текстуры GPU
    int real_width = tex->actual_width;
    int real_height = tex->actual_height;

    sprite_rect_t sprite = png_create_sprite_rect(tex, 0, 0, real_width, real_height);
    int x = (SCREEN_WIDTH - real_width) / 2;
    int y = (SCREEN_HEIGHT - real_height) / 2;
    png_draw_sprite(tex, &sprite, x, y, real_width, real_height);
    graphics_flush_batch();
}

void splash_update_nokia(void) {
    // Увеличиваем таймер
    g_game.splash_timer++;

    // Переход к Bounce splash через 90 кадров (3 секунды при 30fps)
    if (g_game.splash_timer >= 90 || input_pressed(PSP_CTRL_CROSS) || input_pressed(PSP_CTRL_START)) {
        g_game.state = STATE_SPLASH;
        g_game.splash_timer = 0;
    }
}

void splash_update_bounce(void) {
    // Переход к меню только по START (убрали таймер и X)
    if (input_pressed(PSP_CTRL_START)) {
        g_game.state = STATE_MENU;
        g_game.splash_timer = 0;
    }
}

void splash_render_nokia(void) {
    graphics_clear(COLOR_TEXT_NORMAL);
    graphics_begin_plain();
    {
        const char *title = "GITHUB";
        const char *name = "github.com/M-Essa11";
        int title_width = graphics_measure_text(title, 23);
        int name_width = graphics_measure_text(name, 12);
        graphics_draw_text((SCREEN_WIDTH - title_width) / 2, 105, title, COLOR_WHITE_ABGR, 23);
        graphics_draw_text((SCREEN_WIDTH - name_width) / 2, 140, name, COLOR_WHITE_ABGR, 12);
    }
}

void splash_render_bounce(void) {
    // Тот же фон что у About экрана
    graphics_clear(BACKGROUND_COLOUR);

    draw_centered_splash(g_game.bounce_splash_texture);

    graphics_begin_plain();

    // Надпись "Press START" под PNG
    int text_y = 242;
    const char* press_start_text = local_text_press_start();

    // Центрируем текст по горизонтали
    int text_width = graphics_measure_text(press_start_text, 12);
    int text_x = (SCREEN_WIDTH - text_width) / 2;

    graphics_draw_text(text_x, text_y, press_start_text, COLOR_WHITE_ABGR, 12);
}
