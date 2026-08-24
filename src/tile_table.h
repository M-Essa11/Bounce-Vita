#ifndef TILE_TABLE_H
#define TILE_TABLE_H

#include "platform.h"

// КРИТИЧЕСКИ ВАЖНО: TILE_SIZE = 12 - архитектурный инвариант
// Все маски коллизий, спрайты и логика игры привязана к 12×12 пикселям из оригинала Bounce
// Изменение потребует полной регенерации ресурсов в level_masks.inc и перекодирования физики
#define TILE_SIZE 12


// Категории тайлов (соответствуют константам BounceConst.java)
typedef enum {
    EMPTY_SPACE = 0,           // ID_EMPTY_SPACE
    BRICK_RED,                 // ID_BRICK_RED
    BRICK_BLUE,                // ID_BRICK_BLUE (резиновый)
    SPIKE_FLOOR,               // ID_SPIKE_FLOOR
    SPIKE_LEFT_WALL,           // ID_SPIKE_LEFT_WALL
    SPIKE_CEILING,             // ID_SPIKE_CEILING
    SPIKE_RIGHT_WALL,          // ID_SPIKE_RIGHT_WALL
    RESPAWN_GEM,               // ID_RESPAWN_GEM (чекпоинт)
    RESPAWN_INDICATOR,         // ID_RESPAWN_INDICATOR (активированный чекпоинт)
    EXIT_TILE,                 // ID_EXIT_TILE
    MOVING_SPIKE_TILE,         // ID_MOVING_SPIKE_TILE
    HOOP_ACTIVE,               // ID_HOOP_ACTIVE_* (13-16) - активные кольца
    HOOP_INACTIVE,             // ID_HOOP_INACTIVE_* (17-20) - неактивные кольца  
    LARGE_HOOP_ACTIVE,         // ID_LARGE_HOOP_ACTIVE_* (21-24) - большие активные
    LARGE_HOOP_INACTIVE,       // ID_LARGE_HOOP_INACTIVE_* (25-28) - большие неактивные
    TRIANGLE_FLOOR,            // ID_TRIANGLE_* (рампы)
    EXTRA_LIFE,                // ID_EXTRA_LIFE
    SPEED_BONUS,               // ID_SPEED
    DEFLATOR_TILE,             // ID_DEFLATOR_* (уменьшение)  
    INFLATOR_TILE,             // ID_INFLATOR_* (увеличение)
    GRAVITY_BONUS,             // ID_GRAVITY_* (бонус гравитации)
    JUMP_BONUS,                // ID_JUMP_* (бонус прыжка)
    GENERIC_TILE               // Для остальных/неопределенных
} TileCategory;

// Ориентация тайла
typedef enum {
    ORIENT_NONE = 0,           // Без ориентации
    ORIENT_TL,                 // Top-Left (верхний левый угол)
    ORIENT_TR,                 // Top-Right (верхний правый угол)
    ORIENT_BL,                 // Bottom-Left (нижний левый угол)
    ORIENT_BR,                 // Bottom-Right (нижний правый угол)
    // Ориентации для колец (на основе Java BounceConst)
    ORIENT_VERT_TOP,           // Вертикальное кольцо - верхняя часть
    ORIENT_VERT_BOTTOM,        // Вертикальное кольцо - нижняя часть  
    ORIENT_HORIZ_LEFT,         // Горизонтальное кольцо - левая часть
    ORIENT_HORIZ_RIGHT,        // Горизонтальное кольцо - правая часть
    // Ориентации для шипов с тонкими коллизиями
    ORIENT_SPIKE_THIN_HORIZ,   // Тонкая коллизия горизонтально (для шипов вверх/вниз)
    ORIENT_SPIKE_THIN_VERT     // Тонкая коллизия вертикально (для шипов лево/право)
} TileOrientation;

// Тип коллизии
typedef enum {
    COLLISION_NONE = 0,        // Нет коллизии (проходимый)
    COLLISION_SOLID,           // Полная коллизия (непроходимый блок)
    COLLISION_ORIENTED         // Ориентированная коллизия (используется orientation)
} CollisionType;

// Трансформации спрайтов (на основе Java manipulateImage)
typedef enum {
    TF_NONE = 0,               // Без трансформации
    TF_FLIP_X = 1,             // Отражение по X (manipulateImage case 0)
    TF_FLIP_Y = 2,             // Отражение по Y (manipulateImage case 1) 
    TF_FLIP_XY = 3,            // Отражение по X и Y (manipulateImage case 2)
    TF_ROT_90 = 4,             // Поворот на 90° (manipulateImage case 3)
    TF_ROT_180 = 5,            // Поворот на 180° (manipulateImage case 4)
    TF_ROT_270 = 6,            // Поворот на 270° (manipulateImage case 5)
    // Составные трансформации для Java-совместимости
    TF_ROT_270_FLIP_X = 7,     // ROT_270 + FLIP_X (для Java tileImages[35])
    TF_ROT_270_FLIP_Y = 8,     // ROT_270 + FLIP_Y (для Java tileImages[34])
    TF_ROT_270_FLIP_XY = 9     // ROT_270 + FLIP_X + FLIP_Y
} TileTransform;


// Тип рендеринга тайла
typedef enum {
    RENDER_NORMAL = 0,         // Обычный тайл (базовый спрайт)
    RENDER_COMPOSITE = 1,      // Составной тайл (EXIT, движущиеся шипы)
    // SPECIAL_DUAL_SPRITE = 2 - REMOVED (was deprecated, not used)
    RENDER_HOOP = 8            // Кольцо-обруч (как в Java: add2HoopList)
} TileRenderType;

// Константы ID тайлов (соответствуют оригинальному Java коду)
#define TILE_EMPTY           0
#define TILE_BRICK_RED       1
#define TILE_BRICK_BLUE      2
#define TILE_SPIKE_UP        3
#define TILE_SPIKE_LEFT      4
#define TILE_SPIKE_DOWN      5
#define TILE_SPIKE_RIGHT     6
#define TILE_CHECKPOINT      7
#define TILE_CHECKPOINT_ON   8
#define TILE_EXIT            9
#define TILE_MOVING_SPIKES   10
#define TILE_EXTRA_LIFE      29
#define TILE_SPEED_BONUS     38

// Тайлы уменьшения мяча (deflator)
#define TILE_DEFLATOR_FLOOR      39
#define TILE_DEFLATOR_LEFT_WALL  40
#define TILE_DEFLATOR_CEILING    41
#define TILE_DEFLATOR_RIGHT_WALL 42

// Тайлы увеличения мяча (inflator)
#define TILE_INFLATOR_FLOOR      43
#define TILE_INFLATOR_LEFT_WALL  44
#define TILE_INFLATOR_CEILING    45
#define TILE_INFLATOR_RIGHT_WALL 46

// Тайлы бонуса гравитации
#define TILE_GRAVITY_FLOOR      47
#define TILE_GRAVITY_LEFT_WALL  48
#define TILE_GRAVITY_CEILING    49
#define TILE_GRAVITY_RIGHT_WALL 50

// Тайлы бонуса прыжка
#define TILE_JUMP_FLOOR      51
#define TILE_JUMP_LEFT_WALL  52
#define TILE_JUMP_CEILING    53
#define TILE_JUMP_RIGHT_WALL 54

// Флаги тайлов (соответствуют BounceConst.java)
#define TILE_DIRTY_BIT    0x80   // TILE_DIRTY = 128
#define TILE_CLEAN_MASK   0x7F   // Маска для очистки dirty флага

// Основная структура метаданных тайла
typedef struct {
    TileOrientation orientation;
    CollisionType collision_type;
    uint16_t sprite_index;
    TileTransform transform;
    uint8_t render_type;
} TileMeta;

// Функции доступа к таблице тайлов
const TileMeta* tile_meta_db(void);
uint32_t tile_meta_count(void);


#endif // TILE_TABLE_H
