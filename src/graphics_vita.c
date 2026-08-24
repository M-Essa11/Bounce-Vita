#include "graphics.h"

#include "cbmf.h"
#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <vita2d.h>

#define FONT_COUNT 5

static const char *font_paths[FONT_COUNT] = {
    "fonts/0012-0002.cbmf", "fonts/0015-0064.cbmf", "fonts/0019-0128.cbmf",
    "fonts/0026-0002.cbmf", "fonts/0027-0032.cbmf"
};
static const int font_heights[FONT_COUNT] = {9, 12, 16, 23, 24};
static CbmfFont fonts[FONT_COUNT];
static void *font_data[FONT_COUNT];
static int texturing;
static float vita_scale = 2.0f;

void graphics_vita_set_scale(float scale) { vita_scale = scale; }
float graphics_vita_get_scale(void) { return vita_scale; }

static int font_index(int height) {
    int index;
    for (index = 0; index < FONT_COUNT; ++index) if (font_heights[index] == height) return index;
    return 0;
}

static void load_fonts(void) {
    int index;
    for (index = 0; index < FONT_COUNT; ++index) {
        FILE *file = util_open_file(font_paths[index], "rb");
        long size;
        if (!file) continue;
        fseek(file, 0, SEEK_END); size = ftell(file); fseek(file, 0, SEEK_SET);
        if (size <= 0) { fclose(file); continue; }
        font_data[index] = malloc((size_t)size);
        if (font_data[index] && fread(font_data[index], 1, (size_t)size, file) == (size_t)size)
            cbmf_mount(&fonts[index], font_data[index], (size_t)size);
        fclose(file);
    }
}

void graphics_init(void) { vita2d_init(); load_fonts(); }
void graphics_shutdown(void) {
    int index;
    for (index = 0; index < FONT_COUNT; ++index) free(font_data[index]);
    vita2d_fini();
}
void graphics_start_frame(void) { vita2d_start_drawing(); }
void graphics_end_frame(void) { vita2d_end_drawing(); vita2d_swap_buffers(); vita2d_wait_rendering_done(); }
void graphics_set_scissor_fullscreen(void) { vita2d_disable_clipping(); }
void graphics_clear(u32 color) { vita2d_set_clear_color(color); vita2d_clear_screen(); }
void graphics_draw_rect(int x, int y, int w, int h, u32 color) {
    vita2d_draw_rectangle(x * vita_scale, y * vita_scale, w * vita_scale, h * vita_scale, color);
}

void graphics_draw_text(int x, int y, const char *text, u32 color, int height) {
    const CbmfFont *font = &fonts[font_index(height)];
    const unsigned char *cursor = (const unsigned char *)text;
    int pen_x = x;
    if (!text || !font->base) return;
    while (*cursor) {
        CbmfGlyphView glyph;
        unsigned int codepoint = *cursor++;
        int row, column;
        if (codepoint >= 128) continue;
        if (cbmf_get_glyph(font, codepoint, &glyph) != CBMF_OK) continue;
        for (row = 0; row < glyph.height; ++row) {
            for (column = 0; column < glyph.width; ++column) {
                unsigned char packed = glyph.bitmap[row * glyph.row_bytes + (column >> 1)];
                unsigned int coverage = (column & 1) ? (packed >> 4) : (packed & 15);
                unsigned int alpha;
                if (!coverage) continue;
                alpha = color >> 24;
                vita2d_draw_rectangle((pen_x + glyph.offset_x + column) * vita_scale,
                    (y + glyph.offset_y + row) * vita_scale, vita_scale, vita_scale,
                    (color & 0x00FFFFFF) | (alpha << 24));
            }
        }
        pen_x += glyph.advance_x;
    }
}

int graphics_measure_text(const char *text, int height) {
    int32_t width = 0;
    CbmfFont *font = &fonts[font_index(height)];
    if (!text || !font->base) return 0;
    cbmf_text_width_utf8(font, text, &width);
    return (int)width;
}
void graphics_draw_number(int x, int y, int number, u32 color) {
    char text[32]; snprintf(text, sizeof(text), "%d", number); graphics_draw_text(x, y, text, color, 24);
}
int graphics_measure_number(int number) {
    char text[32]; snprintf(text, sizeof(text), "%d", number); return graphics_measure_text(text, 24);
}
void graphics_set_texturing(int enabled) { texturing = enabled != 0; }
void graphics_begin_plain(void) { texturing = 0; }
void graphics_begin_textured(void) { texturing = 1; }
int graphics_get_texturing_state(void) { return texturing; }
void graphics_bind_texture(texture_t *texture) { (void)texture; }
void graphics_batch_sprite(int u1, int v1, int u2, int v2, int x, int y, int w, int h) {
    (void)u1; (void)v1; (void)u2; (void)v2; (void)x; (void)y; (void)w; (void)h;
}
void graphics_batch_sprite_colored(int u1, int v1, int u2, int v2, int x, int y, int w, int h, u32 color) {
    (void)u1; (void)v1; (void)u2; (void)v2; (void)x; (void)y; (void)w; (void)h; (void)color;
}
void graphics_flush_batch(void) {}
