#include "types.h"
#include "level.h"
#include "tile_table.h"
#include "game.h"
#include <assert.h>
#include <string.h>

Level g_level;
Game g_game;
const TileMeta* tile_meta_db(void) { return NULL; }
uint32_t tile_meta_count(void) { return 64; }
int level_find_moving_object_at(int x, int y) { (void)x; (void)y; return -1; }
MovingObject* level_get_moving_object(int i) { (void)i; return NULL; }
void level_set_id(int x, int y, uint8_t id) { g_level.tileMap[y][x] = id; }
void game_add_score(int n) { (void)n; }
void game_add_ring(void) {}
void game_ring_collected(int x, int y, uint8_t id) { (void)x; (void)y; (void)id; }
void game_set_respawn(int x, int y) { (void)x; (void)y; }
void game_add_extra_life(void) {}
void game_complete_level(void) {}
void sound_play_pop(void) {}
void sound_play_pickup(void) {}
void sound_play_hoop(void) {}
bool game_exit_is_open(void) { return false; }

static void test_uphill_traversal(uint8_t tile, BallSizeState size, int direction) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = tile;

    Player p;
    int startX = size == SMALL_SIZE_STATE
        ? (direction > 0 ? 42 : 66)
        : (direction > 0 ? 40 : 68);
    player_init(&p, startX, 53, size);
    p.xSpeed = direction * 40;
    p.ySpeed = 0;
    p.mGroundedFlag = false;
    set_direction(&p, direction > 0 ? MOVE_RIGHT : MOVE_LEFT);

    for (int tick = 0; tick < 3; tick++) {
        player_update(&p);
    }

    assert(direction * (p.xPos - startX) >= 8);
    assert(p.yPos < 53);
}

static void test_airborne_ramp_redirect(void) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = 32;

    Player p;
    player_init(&p, 54, 45, SMALL_SIZE_STATE);
    p.xSpeed = 20;
    p.ySpeed = 40;
    p.mGroundedFlag = false;
    player_update(&p);

    // A vertical ramp impact still redirects velocity (unlike an X traversal probe).
    assert(p.xSpeed < 0);
    assert(p.ySpeed > 0);
}

static void test_blocked_ramp_side_hit(uint8_t tile, int expectedYSpeed) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = tile;

    Player p;
    player_init(&p, 66, 54, SMALL_SIZE_STATE);
    p.xSpeed = -40;
    p.ySpeed = 0;
    p.mGroundedFlag = false;
    player_update(&p);

    assert(p.xPos == 68);
    assert(p.xSpeed == (tile == 32 ? 1 : 2));
    assert(p.ySpeed == expectedYSpeed);
}

static void test_grounded_blocked_ramp_reverses_without_redirect(void) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = 32;

    Player p;
    player_init(&p, 66, 54, SMALL_SIZE_STATE);
    p.xSpeed = -40;
    p.ySpeed = 0;
    p.mGroundedFlag = true;
    player_update(&p);

    assert(p.xSpeed > 0);
    assert(p.ySpeed == 0);
}

static void test_reverse_gravity_vertical_collision(int startY, int ySpeed, bool jump, bool roof) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = 1;

    Player p;
    player_init(&p, 54, startY, SMALL_SIZE_STATE);
    p.ySpeed = ySpeed;
    p.gravBonusCntr = 10;
    if (roof) p.mGroundedFlag = false;
    if (jump) set_direction(&p, MOVE_UP);
    player_update(&p);

    if (roof) {
        assert(!p.mGroundedFlag);
        assert(p.ySpeed < 0);
    } else if (jump) {
        assert(!p.mGroundedFlag);
        assert(p.ySpeed > MIN_BOUNCE_SPEED);
    } else {
        assert(p.mGroundedFlag);
        assert(p.ySpeed > 0);
    }
}

static void test_solid_obstacle_bounce(void) {
    memset(&g_level, 0, sizeof(g_level));
    g_level.width = 12;
    g_level.height = 10;
    g_level.tileMap[4][4] = 1;

    Player p;
    player_init(&p, 54, 42, SMALL_SIZE_STATE);
    p.ySpeed = 40;
    player_update(&p);

    assert(p.mGroundedFlag);
    assert(p.ySpeed < 0);
}

int main(void) {
    test_uphill_traversal(32, SMALL_SIZE_STATE, 1);
    test_uphill_traversal(33, SMALL_SIZE_STATE, -1);
    test_uphill_traversal(36, SMALL_SIZE_STATE, 1);
    test_uphill_traversal(37, SMALL_SIZE_STATE, -1);
    test_uphill_traversal(32, LARGE_SIZE_STATE, 1);
    test_uphill_traversal(33, LARGE_SIZE_STATE, -1);
    test_uphill_traversal(36, LARGE_SIZE_STATE, 1);
    test_uphill_traversal(37, LARGE_SIZE_STATE, -1);
    test_airborne_ramp_redirect();
    test_blocked_ramp_side_hit(32, 18);
    test_blocked_ramp_side_hit(36, 36);
    test_grounded_blocked_ramp_reverses_without_redirect();
    test_reverse_gravity_vertical_collision(42, 40, false, true);
    test_reverse_gravity_vertical_collision(42, 40, true, true);
    test_reverse_gravity_vertical_collision(66, -40, false, false);
    test_reverse_gravity_vertical_collision(66, -40, true, false);
    test_solid_obstacle_bounce();
    return 0;
}
