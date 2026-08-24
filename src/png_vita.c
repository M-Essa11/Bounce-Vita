#include "png.h"

#include "graphics.h"
#include "types.h"
#include <psp2/gxm.h>
#include <stdlib.h>
#include <vita2d.h>

texture_t *png_load_texture_vram(const char *path) {
    FILE *file = util_open_file(path, "rb");
    long size;
    void *buffer;
    vita2d_texture *native;
    texture_t *texture;

    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size <= 0) { fclose(file); return NULL; }
    buffer = malloc((size_t)size);
    if (!buffer) { fclose(file); return NULL; }
    if (fread(buffer, 1, (size_t)size, file) != (size_t)size) {
        free(buffer); fclose(file); return NULL;
    }
    fclose(file);

    native = vita2d_load_PNG_buffer(buffer);
    free(buffer);
    if (!native) return NULL;
    vita2d_texture_set_filters(native, SCE_GXM_TEXTURE_FILTER_POINT, SCE_GXM_TEXTURE_FILTER_POINT);

    texture = calloc(1, sizeof(*texture));
    if (!texture) { vita2d_free_texture(native); return NULL; }
    texture->data = native;
    texture->width = (int)vita2d_texture_get_width(native);
    texture->height = (int)vita2d_texture_get_height(native);
    texture->actual_width = texture->width;
    texture->actual_height = texture->height;
    texture->is_vram = 1;
    texture->source_scale = 1;
    return texture;
}

sprite_rect_t png_create_sprite_rect(texture_t *texture, int x, int y, int w, int h) {
    int scale = texture && texture->source_scale > 0 ? texture->source_scale : 1;
    sprite_rect_t rectangle = {x * scale, y * scale, w * scale, h * scale};
    return rectangle;
}

void png_draw_sprite(texture_t *texture, sprite_rect_t *sprite, int x, int y, int w, int h) {
    float scale = graphics_vita_get_scale();
    if (!texture || !texture->data || !sprite) return;
    vita2d_draw_texture_part_scale((vita2d_texture *)texture->data,
        x * scale, y * scale,
        sprite->x, sprite->y, sprite->w, sprite->h,
        (w * scale) / sprite->w, (h * scale) / sprite->h);
}

void png_free_texture(texture_t *texture) {
    if (!texture) return;
    if (texture->data) vita2d_free_texture((vita2d_texture *)texture->data);
    free(texture);
}

void png_draw_sprite_uv4(texture_t *texture,
    int u_tl, int v_tl, int u_tr, int v_tr,
    int u_bl, int v_bl, int u_br, int v_br,
    int x, int y, int w, int h) {
    vita2d_texture_vertex *vertices;
    float inverse_width, inverse_height;
    float scale;

    if (!texture || !texture->data || texture->width <= 0 || texture->height <= 0 ||
        w <= 0 || h <= 0) return;

    vertices = vita2d_pool_memalign(sizeof(*vertices) * 4, sizeof(*vertices));
    if (!vertices) return;

    inverse_width = 1.0f / texture->width;
    inverse_height = 1.0f / texture->height;
    scale = graphics_vita_get_scale();
    vertices[0] = (vita2d_texture_vertex){x * scale, y * scale, 0.5f,
        u_tl * inverse_width, v_tl * inverse_height};
    vertices[1] = (vita2d_texture_vertex){(x + w) * scale, y * scale, 0.5f,
        u_tr * inverse_width, v_tr * inverse_height};
    vertices[2] = (vita2d_texture_vertex){x * scale, (y + h) * scale, 0.5f,
        u_bl * inverse_width, v_bl * inverse_height};
    vertices[3] = (vita2d_texture_vertex){(x + w) * scale, (y + h) * scale, 0.5f,
        u_br * inverse_width, v_br * inverse_height};

    vita2d_draw_array_textured((vita2d_texture *)texture->data,
        SCE_GXM_PRIMITIVE_TRIANGLE_STRIP, vertices, 4, 0xFFFFFFFF);
}

void png_draw_sprite_transform(texture_t *texture, sprite_rect_t *sprite,
    int x, int y, int w, int h, png_transform_t transform) {
    int u1, v1, u2, v2, indices[4] = {0, 1, 2, 3};
    int u[4], v[4], rotation = 0, flip_x = 0, flip_y = 0;
    if (!texture || !sprite) return;
    u1 = sprite->x; v1 = sprite->y; u2 = u1 + sprite->w; v2 = v1 + sprite->h;
    u[0] = u1; u[1] = u2; u[2] = u1; u[3] = u2;
    v[0] = v1; v[1] = v1; v[2] = v2; v[3] = v2;
    switch (transform) {
        case PNG_TRANSFORM_ROT_90: rotation = 1; break;
        case PNG_TRANSFORM_ROT_180: rotation = 2; break;
        case PNG_TRANSFORM_ROT_270: rotation = 3; break;
        case PNG_TRANSFORM_FLIP_X: flip_x = 1; break;
        case PNG_TRANSFORM_FLIP_Y: flip_y = 1; break;
        case PNG_TRANSFORM_ROT_270_FLIP_X: rotation = 3; flip_x = 1; break;
        case PNG_TRANSFORM_ROT_270_FLIP_Y: rotation = 3; flip_y = 1; break;
        case PNG_TRANSFORM_ROT_270_FLIP_XY: rotation = 3; flip_x = flip_y = 1; break;
        default: break;
    }
    while (rotation--) {
        int a = indices[0], b = indices[1], c = indices[2], d = indices[3];
        indices[0] = b; indices[1] = d; indices[2] = a; indices[3] = c;
    }
    if (flip_x) {
        int t = indices[0]; indices[0] = indices[1]; indices[1] = t;
        t = indices[2]; indices[2] = indices[3]; indices[3] = t;
    }
    if (flip_y) {
        int t = indices[0]; indices[0] = indices[2]; indices[2] = t;
        t = indices[1]; indices[1] = indices[3]; indices[3] = t;
    }
    png_draw_sprite_uv4(texture,
        u[indices[0]], v[indices[0]], u[indices[1]], v[indices[1]],
        u[indices[2]], v[indices[2]], u[indices[3]], v[indices[3]], x, y, w, h);
}
