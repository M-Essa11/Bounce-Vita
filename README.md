# Bounce Vita

Bounce Vita is a native PlayStation Vita port of the classic Nokia Bounce game. It is based on the open-source C reimplementation from [amdray/bounce_zero](https://github.com/amdray/bounce_zero), adapted for VitaSDK with Vita2D rendering, Vita controls, audio, persistent progress, and refreshed presentation assets.

## Features

- Native PlayStation Vita VPK
- Vita2D hardware-accelerated rendering
- All 11 classic levels
- Music and sound effects
- Persistent level, score, and lives
- Refreshed ball, obstacle, HUD, menu, splash, and LiveArea graphics
- English and original game localization data

## Controls

| Control | Action |
| --- | --- |
| D-pad / Left stick | Move and navigate |
| Cross | Jump / confirm |
| Circle | Back |
| Start | Pause |
| L trigger | Move left |
| R trigger | Move right |
| Square + Triangle | Toggle invincibility cheat |

## Save Data

Progress is stored at:

```text
ux0:data/BounceVita/save.dat
```


## Building

Install [VitaSDK](https://vitasdk.org/) and its Vita2D dependency, then export `VITASDK` and add its tools to `PATH`.

On Ubuntu or WSL:

```sh
export VITASDK="$HOME/vitasdk"
export PATH="$VITASDK/bin:$PATH"
./build-vita.sh
```

The package is created at:

```text
build-vita/bounce_zero_vita.vpk
```

## Installation

Transfer `bounce_zero_vita.vpk` to a homebrew-enabled PlayStation Vita and install it with VitaShell.

## Credits

- Original Bounce game: Nokia / Sun Microsystems
- Original C reimplementation: [amdray/bounce_zero](https://github.com/amdray/bounce_zero)
- PlayStation Vita port and presentation updates: [M-Essa11](https://github.com/M-Essa11)
- VitaSDK and Vita2D contributors

## Legal

The source code is available under the MIT License; see `LICENSE`. Bounce names, characters, and original game assets remain the property of their respective owners. This fan project is not affiliated with or endorsed by Nokia, Sun Microsystems, Sony, or PlayStation.

