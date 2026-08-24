#!/usr/bin/env python3
import sys
from collections import deque
from pathlib import Path

from PIL import Image


def is_checker(pixel):
    red, green, blue, _ = pixel
    return max(red, green, blue) - min(red, green, blue) <= 10 and min(red, green, blue) >= 190


def clear_connected_background(tile):
    pixels = tile.load()
    width, height = tile.size
    queue = deque()
    visited = set()
    for x in range(width):
        queue.append((x, 0))
        queue.append((x, height - 1))
    for y in range(height):
        queue.append((0, y))
        queue.append((width - 1, y))
    while queue:
        point = queue.popleft()
        if point in visited:
            continue
        visited.add(point)
        x, y = point
        if not is_checker(pixels[x, y]):
            continue
        pixels[x, y] = (0, 0, 0, 0)
        if x > 0:
            queue.append((x - 1, y))
        if x + 1 < width:
            queue.append((x + 1, y))
        if y > 0:
            queue.append((x, y - 1))
        if y + 1 < height:
            queue.append((x, y + 1))


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: prepare_hd_atlas.py INPUT OUTPUT")
    source = Image.open(sys.argv[1]).convert("RGBA")
    if source.size != (1024, 1536):
        raise SystemExit(f"Expected a 1024x1536 source atlas, got {source.size}")
    atlas = Image.new("RGBA", (192, 288))
    for row in range(6):
        for column in range(4):
            tile = source.crop((column * 256, row * 256, (column + 1) * 256, (row + 1) * 256))
            clear_connected_background(tile)
            tile = tile.resize((48, 48), Image.Resampling.LANCZOS)
            atlas.alpha_composite(tile, (column * 48, row * 48))
    output = Path(sys.argv[2])
    output.parent.mkdir(parents=True, exist_ok=True)
    atlas.save(output, optimize=True)


if __name__ == "__main__":
    main()
