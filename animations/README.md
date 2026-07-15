# Optional high-resolution explainers

The committed `examples/*-explainer.gif` files are 960 × 540 previews rendered
from these scenes. They are the README presentation assets; the C toolkit's
SVG exporters remain available for diagnostics, not for README presentation.

`toolkit_stories.py` is a Manim Community companion for rendering the binary,
list, and sketch explainers as high-resolution MP4 video. It is deliberately outside the
CMake build and CI, so the portable C23 toolkit does not require Python,
FFmpeg, Pango, OpenGL, or a video renderer.

Install the prerequisites appropriate for your platform, then create an
isolated environment and render the committed GIF previews:

```bash
python3 -m venv .venv
. .venv/bin/activate
pip install -r animations/requirements.txt
MANIM="$PWD/.venv/bin/manim" ./animations/render.sh
```

The script invokes Manim with `-qh` (1080p/60fps), retains its MP4 files below
`media/manim/`, and derives 960 × 540 GIF previews. `media/` is ignored because
video files are derived output.

- **Binary** uses the `i8 -12` fixture and exact two's-complement bits from the
  C binary visualizer.
- **Lists** uses the exact four-operation sequence printed by `list-demo`.
- **Sketches** begins with the actual `gallery.sk` byte stream, then pairs each
  of the six C decoder frames with its matching `sketch-inspect` event.
- **Renderer** captures `donut-animate --incremental 80 24`, strips only ANSI
  cursor-control sequences, and encodes its real 80 × 24 ASCII frames directly
  as the standalone README GIF—without Manim.
