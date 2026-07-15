# Sketch stream format

The decoder consumes one command per byte. The top two bits select an opcode;
the low six bits are the operand.

| Bits 7–6 | Opcode | Operand interpretation |
| --- | --- | --- |
| `00` | `TOOL` | Unsigned tool command |
| `01` | `DX` | Signed 6-bit delta for the target x-coordinate |
| `10` | `DY` | Signed 6-bit delta for the target y-coordinate, then draw/move |
| `11` | `DATA` | Unsigned six-bit group appended to the current data value |

Signed six-bit operands range from `-32` through `31`. `DATA` groups are
assembled most-significant group first; a `TOOL` command consumes and clears
the accumulated value.

## Tool commands

| Value | Command | Effect |
| ---: | --- | --- |
| 0 | `NONE` | Move without drawing. |
| 1 | `LINE` | Draw a Bresenham line when a `DY` command completes a move. |
| 2 | `BLOCK` | Fill the inclusive rectangle between current and target points. |
| 3 | `COLOUR` | Set grayscale colour from `DATA` (`0` black, `255` white). |
| 4 | `TARGET_X` | Set target x-coordinate from `DATA`. |
| 5 | `TARGET_Y` | Set target y-coordinate from `DATA`. |
| 6 | `SHOW` | Presentation marker; retained as a no-op in non-interactive output. |
| 7 | `PAUSE` | Presentation marker; retained as a no-op in non-interactive output. |
| 8 | `NEXT_FRAME` | Frame marker; the decoder continues rendering the stream. |

The initial state is `(0, 0)` with target `(0, 0)`, black line drawing, and a
white canvas. Coordinates outside the canvas are clipped at pixel-write time.
Unknown tool commands, values that overflow the decoder state, and colour or
coordinate values outside their supported ranges fail cleanly.
