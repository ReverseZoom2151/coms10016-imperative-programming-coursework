# Visual stories

`toolkit-visualize` produces deterministic diagnostic media from actual toolkit
operations. It writes an explanatory SVG and a GIF that shows the state
sequence; the README uses corresponding high-resolution explainers from
[`animations/`](../animations/).

```bash
./build/toolkit-visualize binary build/binary-story.svg build/binary-story.gif
./build/toolkit-visualize list build/list-story.svg build/list-story.gif
./build/toolkit-visualize sketch examples/gallery.sk build/sketch-story.svg build/sketch-story.gif
./build/toolkit-visualize renderer build/renderer-story.svg build/renderer-story.gif
```

- **Binary** uses the library’s signed encoder for `i8 -12`; the diagnostic
  GIF reveals the stored bits in order and the SVG identifies the sign bit and
  hexadecimal result.
- **Lists** executes the same mutation sequence as `list-demo`; the cursor is
  shown at every resulting state.
- **Sketches** selects drawing-triggering bytes across the supplied stream and
  decodes every prefix independently. Each frame is therefore a genuine
  decoder result, not a redraw approximation.
- **Renderer** emits four incremental-rotation checkpoints and a 24-frame
  depth-buffered animation.

The visualizer intentionally remains a terminal tool and depends only on the
toolkit’s canvas and export code. Its SVG output is for vector diagnostics, not
for README presentation; the animation companion supplies the polished motion
explainers.
