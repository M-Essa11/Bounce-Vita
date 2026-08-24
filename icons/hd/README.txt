Optional Vita HD texture pack

Place the finished 192x288 RGBA atlas here as objects_hd.png.
The game validates its dimensions and falls back to ../objects_nm.png if it is
missing or invalid. Each atlas cell is 48x48 and corresponds exactly to one
12x12 cell in the classic 4x6 atlas.

Export editable cells:
  python tools/hd_atlas.py export icons/objects_nm.png hd-work

After editing every tile without changing its canvas size, rebuild the atlas:
  python tools/hd_atlas.py rebuild hd-work icons/hd/objects_hd.png
