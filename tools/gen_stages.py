#!/usr/bin/env python3
"""Generate Arashi stage layouts as plain-text files.

Each stage file has FIELD_ROWS (37) lines, one per play-field row. Console row
= field row + 3 (rows 0-2 on screen are the HUD). Characters:
    '.'  side / bottom border
    '='  solid floor or platform you can stand on
    '#'  dirt under the ground
    ' '  open air

The C++ game reads stages\\stageN.txt; the screen shows a VIEW_W (120) wide
slice ("section"), so a wider file lets the player scroll left/right. This
script mirrors buildDefaultField() in arashi.cpp so disk and fallback agree.
"""

VIEW_W = 120
FIELD_ROWS = 37
GROUND = 33          # field row of the ground (console row 36)


def build(stage: int) -> list[str]:
    w = VIEW_W * 2 if stage % 2 == 0 else VIEW_W   # even stages are 2 screens wide
    rows = []
    for r in range(0, 33):                          # open play rows with edge walls
        row = [' '] * w
        row[0] = '.'
        row[w - 1] = '.'
        rows.append(row)
    rows.append(['='] * w)                          # ground   (row 33 / console 36)
    rows.append(['#'] * w)                          # dirt
    rows.append(['#'] * w)                          # dirt
    rows.append(['-'] * w)                          # bottom border

    shift = (stage * 7) % 24                         # platforms, varied per stage
    for x in range(4 + shift, min(30 + shift, w - 1)):
        rows[11][x] = '='
    for x in range(72, w - 2):
        rows[11][x] = '='
    for x in range(4, 40):
        rows[23][x] = '='
    for x in range(78 + shift // 2, w - 2):
        rows[23][x] = '='

    return [''.join(r) for r in rows]


def main():
    import os
    out = os.path.join(os.path.dirname(__file__), '..', 'stages')
    os.makedirs(out, exist_ok=True)
    for s in range(1, 4):                            # STAGE_FILES = 3
        path = os.path.join(out, f'stage{s}.txt')
        with open(path, 'w', newline='\n') as f:
            f.write('\n'.join(build(s)) + '\n')
        print(f'wrote {path} ({"240" if s % 2 == 0 else "120"} wide)')


if __name__ == '__main__':
    main()
