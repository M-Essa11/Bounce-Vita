#include "types.h"

#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <string.h>

static SaveData save_data;
static int dirty;
static const char *save_path = "ux0:data/BounceVita/save.dat";
static const char *legacy_save_path = "ux0:data/BounceZeroVita/save.dat";

void save_init(void) {
    FILE *file;
    memset(&save_data, 0, sizeof(save_data));
    save_data.magic = 0x424F554E;
    save_data.best_level = 1;
    save_data.current_level = 1;
    save_data.current_lives = 3;
    sceIoMkdir("ux0:data/BounceVita", 0777);
    file = fopen(save_path, "rb");
    if (!file) {
        file = fopen(legacy_save_path, "rb");
        if (file) dirty = 1;
    }
    if (file) { fread(&save_data, 1, sizeof(save_data), file); fclose(file); }
    if (save_data.magic != 0x424F554E) {
        memset(&save_data, 0, sizeof(save_data));
        save_data.magic = 0x424F554E;
        save_data.best_level = 1;
    }
    if (save_data.current_level < 1 || save_data.current_level > MAX_LEVEL)
        save_data.current_level = 1;
    if (save_data.current_lives < 0 || save_data.current_lives > 5)
        save_data.current_lives = 3;
    if (dirty) save_flush();
}
void save_flush(void) {
    FILE *file;
    if (!dirty) return;
    file = fopen(save_path, "wb");
    if (file) { fwrite(&save_data, 1, sizeof(save_data), file); fclose(file); dirty = 0; }
}
void save_shutdown(void) { save_flush(); }
void save_update_records(int level, int score) {
    if (level > save_data.best_level) { save_data.best_level = level; dirty = 1; }
    if (score > save_data.best_score) { save_data.best_score = score; dirty = 1; }
}
void save_set_progress(int level, int score, int lives, int active) {
    save_data.current_level = level;
    save_data.current_score = score;
    save_data.current_lives = lives;
    save_data.has_continue = active != 0;
    dirty = 1;
    save_flush();
}
SaveData *save_get_data(void) { return &save_data; }
