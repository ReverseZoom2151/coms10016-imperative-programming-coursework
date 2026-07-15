#!/usr/bin/env bash
set -euo pipefail

# Generate C-derived source frames, render 1080p/60fps explainers, then make
# 960px GIF previews that GitHub can display inline. Set MANIM and
# MANIM_PYTHON for a virtual environment.

readonly manim_command="${MANIM:-manim}"
readonly python_command="${MANIM_PYTHON:-python3}"
readonly media_directory="media/manim"
readonly input_directory="media/manim-input"

cmake -S . -B build -G Ninja
cmake --build build
mkdir -p "${input_directory}/sketch-frames"
./build/toolkit-visualize sketch examples/gallery.sk \
  "${input_directory}/sketch-diagnostic.svg" "${input_directory}/sketch-source.gif"
./build/sketch-inspect examples/gallery.sk > "${input_directory}/sketch.trace"
"${python_command}" animations/prepare_sketch_trace.py \
  "${input_directory}/sketch.trace" "${input_directory}/sketch-trace.json" 6
ffmpeg -y -i "${input_directory}/sketch-source.gif" \
  -vsync 0 -vf "scale=768:480:flags=neighbor" \
  "${input_directory}/sketch-frames/frame-%02d.png"
./build/donut-animate --incremental --frames 16 --fps 1000 80 24 \
  > "${input_directory}/donut.ansi"
"${python_command}" animations/prepare_terminal_frames.py \
  "${input_directory}/donut.ansi" "${input_directory}/donut-frames.json" 24
donut_palette="${input_directory}/donut-palette.png"
ffmpeg -y -framerate 10 -i "${input_directory}/terminal-frames/frame-%02d.png" \
  -vf "fps=10,scale=960:-2:flags=lanczos,palettegen=max_colors=128:stats_mode=diff" \
  "${donut_palette}"
ffmpeg -y -framerate 10 -i "${input_directory}/terminal-frames/frame-%02d.png" \
  -i "${donut_palette}" \
  -lavfi "fps=10,scale=960:-2:flags=lanczos[x];[x][1:v]paletteuse=dither=sierra2_4a" \
  "examples/renderer-explainer.gif"

"${manim_command}" -qh --media_dir "${media_directory}" \
  animations/toolkit_stories.py BinaryStory ListStory SketchStory

for story in BinaryStory ListStory SketchStory; do
  case "${story}" in
    BinaryStory) output="examples/binary-explainer.gif" ;;
    ListStory) output="examples/list-explainer.gif" ;;
    SketchStory) output="examples/sketch-explainer.gif" ;;
  esac

  movie="${media_directory}/videos/toolkit_stories/1080p60/${story}.mp4"
  palette="${media_directory}/${story}-palette.png"
  ffmpeg -y -i "${movie}" \
    -vf "fps=20,scale=960:-2:flags=lanczos,palettegen=max_colors=128:stats_mode=diff" \
    "${palette}"
  ffmpeg -y -i "${movie}" -i "${palette}" \
    -lavfi "fps=20,scale=960:-2:flags=lanczos[x];[x][1:v]paletteuse=dither=sierra2_4a" \
    "${output}"
done
