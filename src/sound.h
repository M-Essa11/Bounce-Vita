#ifndef SOUND_H
#define SOUND_H

#include <stdio.h>
#include <stdint.h>

#define SONGNAME_LEN 64

// Минимальная структура для парсинга OTT
struct ott_note_t {
    int tone;          // нота (0-15)
    int length;        // длительность (0-7) 
    int modifier;      // модификатор ноты (0-3)
    int scale;         // октава (0-3)
    int style;         // стиль воспроизведения (0-3)
    int bpm;           // темп
    int volume;        // громкость (0-15)
};

struct ott_info_t {
    char songname[SONGNAME_LEN];
    int loop;
    int scale;
    int style; 
    int bpm;
    int volume;
    int note_count;
    struct ott_note_t notes[1024]; // массив для хранения распарсенных нот
};

// поддержка PSP Audio - interleaved 16-bit stereo samples
// Совместимо с PSP audio buffer format (left, right channels)
typedef struct {
    short l, r;  // left and right channel samples  
} psp_sample_t;

struct ott_player_t {
    struct ott_info_t *ott_info;
    int current_note;
    float note_time;
    float note_duration;
    float frequency;
    uint32_t phase;        // Fixed-point фазовый накопитель (Q32.32)
    uint32_t phase_inc;    // Fixed-point приращение фазы за сэмпл
    uint16_t env_q15;      // 0..32767, огибающая (Q15)
    int is_playing;
    int sample_rate;
};

// объявления функций
int parse_ott(FILE *in, struct ott_info_t *ott_info);
int get_bits(unsigned char *buffer, int *ptr, int *bitptr, int bits);
int reverse_tempo(int l);
int parse_ringtone(unsigned char *buffer, int ptr, struct ott_info_t *ott_info);

// функции PSP Audio (низкоуровневые)
float ott_tone_to_frequency(int tone, int scale);
float ott_length_to_duration(int length, int bpm);
void ott_player_init(struct ott_player_t *player, struct ott_info_t *ott_info);
void ott_audio_callback(void* buf, unsigned int length, void *userdata);
void ott_player_start(struct ott_player_t *player);
void ott_player_stop(struct ott_player_t *player);

// высокоуровневый API для игры
int sound_init(void);
void sound_shutdown(void);
void sound_play_hoop(void);
void sound_play_pickup(void);
void sound_play_pop(void);
void sound_set_volume(int volume);  // 0x0000 = тишина, 0x8000 = максимум

#endif