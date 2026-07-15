# Visual stories

`toolkit-visualize` produces the README media from actual toolkit operations.
It writes both an explanatory SVG and a GIF that shows the state sequence.

```bash
./build/toolkit-visualize binary examples/binary-story.svg examples/binary-story.gif
./build/toolkit-visualize list examples/list-story.svg examples/list-story.gif
./build/toolkit-visualize sketch examples/gallery.sk examples/sketch-story.svg examples/sketch-story.gif
./build/toolkit-visualize renderer examples/renderer-story.svg examples/renderer-story.gif
```

- **Binary** uses the library’s signed encoder for `i8 -12`; the GIF reveals
  the stored bits in order and the SVG identifies the sign bit and hexadecimal
  result.
- **Lists** executes the same mutation sequence as `list-demo`; the cursor is
  shown at every resulting state.
- **Sketches** selects drawing-triggering bytes across the supplied stream and
  decodes every prefix independently. Each frame is therefore a genuine
  decoder result, not a redraw approximation.
- **Renderer** emits four incremental-rotation checkpoints and a 24-frame
  depth-buffered animation.

The visualizer intentionally remains a terminal tool and depends only on the
toolkit’s canvas and export code. SVG supplies explanatory labels; GIF supplies
the time dimension.
