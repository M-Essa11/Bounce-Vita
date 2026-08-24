#!/usr/bin/env python3
import sys
from pathlib import Path

from PIL import Image


CLASSIC_TILE = 12
HD_TILE = 48
CORONA_TILE = 160
CORONA_COLUMNS = 12


def main():
    if len(sys.argv) != 4:
        raise SystemExit("usage: build_corona_hybrid.py CLASSIC_ATLAS CORONA_ASSETS OUTPUT")

    classic = Image.open(sys.argv[1]).convert("RGBA")
    assets = Path(sys.argv[2])
    corona = Image.open(assets / "graphics/map_tiles@4x.png").convert("RGBA")
    corona_ball = Image.open(assets / "images/ball_small@4x.png").convert("RGBA")
    corona_big_ball = Image.open(assets / "images/ball_big@4x.png").convert("RGBA")
    corona_pop_ball = Image.open(assets / "images/ball_pop@4x.png").convert("RGBA")
    corona_thorn = Image.open(assets / "images/thorn@4x.png").convert("RGBA")
    corona_checkpoint = Image.open(assets / "images/checkpoint@4x.png").convert("RGBA")
    corona_pumper = Image.open(assets / "images/pumper@4x.png").convert("RGBA")
    if classic.size != (48, 72):
        raise SystemExit(f"Classic atlas must be 48x72, got {classic.size}")
    if corona.size != (1920, 1760):
        raise SystemExit(f"Corona 4x tileset must be 1920x1760, got {corona.size}")
    if corona_ball.size != (160, 160):
        raise SystemExit(f"Corona 4x small ball must be 160x160, got {corona_ball.size}")
    if corona_big_ball.size != (216, 216):
        raise SystemExit(f"Corona 4x big ball must be 216x216, got {corona_big_ball.size}")
    if corona_pop_ball.size != (160, 160):
        raise SystemExit(f"Corona 4x popped ball must be 160x160, got {corona_pop_ball.size}")

    hybrid = classic.resize((192, 288), Image.Resampling.NEAREST)
    pixels = hybrid.load()
    for y in range(hybrid.height):
        for x in range(hybrid.width):
            red, green, blue, alpha = pixels[x, y]
            if green > 220 and red < 180 and blue < 180:
                pixels[x, y] = (0, 0, 0, 0)
    corona_index = 13
    corona_column = corona_index % CORONA_COLUMNS
    corona_row = corona_index // CORONA_COLUMNS
    brick = corona.crop((corona_column * CORONA_TILE, corona_row * CORONA_TILE,
                         (corona_column + 1) * CORONA_TILE, (corona_row + 1) * CORONA_TILE))
    brick = brick.resize((HD_TILE, HD_TILE), Image.Resampling.LANCZOS)

    classic_brick_column = 1
    classic_brick_row = 0
    hybrid.paste((0, 0, 0, 0),
                 (classic_brick_column * HD_TILE, classic_brick_row * HD_TILE,
                  (classic_brick_column + 1) * HD_TILE, (classic_brick_row + 1) * HD_TILE))
    hybrid.alpha_composite(brick, (classic_brick_column * HD_TILE, classic_brick_row * HD_TILE))

    ball = corona_ball.resize((40, 40), Image.Resampling.LANCZOS)
    ball_cell = Image.new("RGBA", (HD_TILE, HD_TILE))
    ball_cell.alpha_composite(ball, (4, 4))
    classic_ball_column = 2
    classic_ball_row = 0
    hybrid.paste((0, 0, 0, 0),
                 (classic_ball_column * HD_TILE, classic_ball_row * HD_TILE,
                  (classic_ball_column + 1) * HD_TILE, (classic_ball_row + 1) * HD_TILE))
    hybrid.alpha_composite(ball_cell, (classic_ball_column * HD_TILE, classic_ball_row * HD_TILE))

    big_ball = corona_big_ball.resize((64, 64), Image.Resampling.LANCZOS)
    big_ball_quadrant = Image.new("RGBA", (HD_TILE, HD_TILE))
    big_ball_quadrant.alpha_composite(big_ball.crop((0, 0, 32, 32)), (16, 16))
    hybrid.paste((0, 0, 0, 0), (3 * HD_TILE, 0, 4 * HD_TILE, HD_TILE))
    hybrid.alpha_composite(big_ball_quadrant, (3 * HD_TILE, 0))

    popped_ball = corona_pop_ball.resize((HD_TILE, HD_TILE), Image.Resampling.LANCZOS)
    hybrid.paste((0, 0, 0, 0), (0, HD_TILE, HD_TILE, HD_TILE * 2))
    hybrid.alpha_composite(popped_ball, (0, HD_TILE))

    def replace_fit(index, image, maximum_width=HD_TILE, maximum_height=HD_TILE):
        image.thumbnail((maximum_width, maximum_height), Image.Resampling.LANCZOS)
        cell = Image.new("RGBA", (HD_TILE, HD_TILE))
        cell.alpha_composite(image, ((HD_TILE - image.width) // 2, (HD_TILE - image.height) // 2))
        column = index % 4
        row = index // 4
        hybrid.paste((0, 0, 0, 0),
                     (column * HD_TILE, row * HD_TILE,
                      (column + 1) * HD_TILE, (row + 1) * HD_TILE))
        hybrid.alpha_composite(cell, (column * HD_TILE, row * HD_TILE))

    replace_fit(12, corona_thorn)
    replace_fit(16, corona_checkpoint)
    replace_fit(18, corona_pumper)

    def corona_image(name):
        return Image.open(assets / f"images/{name}@4x.png").convert("RGBA")

    replace_fit(5, corona_image("power_speed"), 42, 42)
    replace_fit(7, corona_image("deflater"), 42, 46)
    replace_fit(10, corona_image("power_jump"), 42, 42)
    replace_fit(11, corona_image("power_gravity"), 42, 42)
    replace_fit(15, corona_image("life"), 40, 40)
    replace_fit(17, corona_image("gbar_ring"), 32, 40)
    replace_fit(19, corona_image("checkpoint_catched"), 44, 44)

    blue_brick_index = 93
    blue_column = blue_brick_index % CORONA_COLUMNS
    blue_row = blue_brick_index // CORONA_COLUMNS
    blue_brick = corona.crop((blue_column * CORONA_TILE, blue_row * CORONA_TILE,
                              (blue_column + 1) * CORONA_TILE, (blue_row + 1) * CORONA_TILE))
    replace_fit(9, blue_brick)

    def replace_corona_tile(classic_index, corona_index):
        column = corona_index % CORONA_COLUMNS
        row = corona_index // CORONA_COLUMNS
        tile = corona.crop((column * CORONA_TILE, row * CORONA_TILE,
                            (column + 1) * CORONA_TILE, (row + 1) * CORONA_TILE))
        replace_fit(classic_index, tile)

    replace_corona_tile(0, 89)
    replace_corona_tile(8, 113)

    output = Path(sys.argv[3])
    output.parent.mkdir(parents=True, exist_ok=True)
    hybrid.save(output, optimize=True)


if __name__ == "__main__":
    main()
