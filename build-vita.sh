#!/usr/bin/env bash
set -euo pipefail

project_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
stage_dir="/home/$(id -un)/bounce-zero-vita-buildsrc"
sdk_dir="${VITASDK:-/home/$(id -un)/vitasdk}"

rm -rf "$stage_dir"
mkdir -p "$stage_dir"
cp "$project_dir/CMakeLists.txt" "$stage_dir/"
cp -a "$project_dir/src" "$stage_dir/"
mkdir -p "$stage_dir/assets"
cp -a "$project_dir/assets/vita" "$stage_dir/assets/"
cp -a "$project_dir/icons" "$project_dir/levels" "$project_dir/sounds" "$project_dir/lang" "$project_dir/fonts" "$stage_dir/"

export VITASDK="$sdk_dir"
export PATH="$VITASDK/bin:$PATH"

cmake -S "$stage_dir" -B "$stage_dir/build-vita"
cmake --build "$stage_dir/build-vita" -j2

mkdir -p "$project_dir/build-vita"
cp "$stage_dir/build-vita/bounce_zero_vita.vpk" "$project_dir/build-vita/"

echo "Built $project_dir/build-vita/bounce_zero_vita.vpk"
