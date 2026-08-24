#!/usr/bin/env python3
import argparse
from pathlib import Path

from PIL import Image


TILE_SIZE = 12
HD_SCALE = 4
GRID_COLUMNS = 4
GRID_ROWS = 6


def export_tiles(source: Path, output: Path) -> None:
    atlas = Image.open(source).convert("RGBA")
    if atlas.size != (GRID_COLUMNS * TILE_SIZE, GRID_ROWS * TILE_SIZE):
        raise SystemExit(f"Expected a 48x72 atlas, got {atlas.width}x{atlas.height}")
    output.mkdir(parents=True, exist_ok=True)
    for row in range(GRID_ROWS):
        for column in range(GRID_COLUMNS):
            tile = atlas.crop((column * TILE_SIZE, row * TILE_SIZE,
                               (column + 1) * TILE_SIZE, (row + 1) * TILE_SIZE))
            tile.resize((TILE_SIZE * HD_SCALE, TILE_SIZE * HD_SCALE), Image.Resampling.NEAREST).save(
                output / f"tile_{row:02d}_{column:02d}.png")


def rebuild_atlas(source: Path, output: Path) -> None:
    tile_size = TILE_SIZE * HD_SCALE
    atlas = Image.new("RGBA", (GRID_COLUMNS * tile_size, GRID_ROWS * tile_size))
    for row in range(GRID_ROWS):
        for column in range(GRID_COLUMNS):
            path = source / f"tile_{row:02d}_{column:02d}.png"
            if not path.is_file():
                raise SystemExit(f"Missing {path}")
            tile = Image.open(path).convert("RGBA")
            if tile.size != (tile_size, tile_size):
                raise SystemExit(f"{path.name} must be 48x48, got {tile.width}x{tile.height}")
            atlas.alpha_composite(tile, (column * tile_size, row * tile_size))
    output.parent.mkdir(parents=True, exist_ok=True)
    atlas.save(output, optimize=True)


def main() -> None:
    parser = argparse.ArgumentParser(description="Export or rebuild Bounce's tile-safe 4x HD atlas")
    subparsers = parser.add_subparsers(dest="command", required=True)
    export_parser = subparsers.add_parser("export")
    export_parser.add_argument("source", type=Path)
    export_parser.add_argument("output", type=Path)
    rebuild_parser = subparsers.add_parser("rebuild")
    rebuild_parser.add_argument("source", type=Path)
    rebuild_parser.add_argument("output", type=Path)
    arguments = parser.parse_args()
    if arguments.command == "export":
        export_tiles(arguments.source, arguments.output)
    else:
        rebuild_atlas(arguments.source, arguments.output)


if __name__ == "__main__":
    main()
