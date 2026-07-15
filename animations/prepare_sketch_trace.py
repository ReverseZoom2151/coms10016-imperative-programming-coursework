"""Select the same decoder events used by toolkit-visualize's sketch GIF."""

import json
import re
import sys
from pathlib import Path


DRAW_EVENT = re.compile(
    r"^(?P<byte>\d+)\s+0x[0-9A-F]+\s+DY\s+(?P<delta>[+-]\d+) -> "
    r"(?P<tool>\w+) \((?P<x0>-?\d+),(?P<y0>-?\d+)\) -> "
    r"\((?P<x1>-?\d+),(?P<y1>-?\d+)\), colour=(?P<colour>\d+)"
)


def main() -> int:
    if len(sys.argv) != 4:
        print("usage: prepare_sketch_trace.py INPUT.trace OUTPUT.json COUNT", file=sys.stderr)
        return 2
    lines = Path(sys.argv[1]).read_text(encoding="utf-8").splitlines()
    events = [match for line in lines if (match := DRAW_EVENT.match(line))]
    count = min(int(sys.argv[3]), len(events))
    if count == 0:
        return 1
    selected = [
        events[index * (len(events) - 1) // (count - 1)] if count > 1 else events[0]
        for index in range(count)
    ]
    summaries = [
        "\n".join(
            [
                f"byte {event['byte']}",
                f"DY {event['delta']} → {event['tool']}",
                f"cursor ({event['x0']},{event['y0']}) → ({event['x1']},{event['y1']})",
                f"colour {event['colour']}",
            ]
        )
        for event in selected
    ]
    Path(sys.argv[2]).write_text(json.dumps(summaries), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
