Arashi stage files
==================

Each stageN.txt is one stage's map. The game cycles through stage1..stage3
(stage 4 reuses stage1's layout, and so on) while the difficulty keeps rising,
so the game is endless.

Format
------
- 37 lines, one per play-field row. On screen, field row r is drawn at console
  row r + 3 (rows 0-2 are the HUD).
- A line may be wider than the 120-column screen. The screen shows one 120-wide
  "section"; walk off the left/right edge to scroll to the neighbouring section.
  A 240-wide file therefore gives a two-screen level (see stage2.txt).
- Characters:
    .   border wall (left/right edges and the bottom)
    =   solid floor / a platform you can stand on or jump onto
    #   dirt beneath the ground (also solid)
    (space)  open air
- Row 34 (1-based) is the main ground and should stay solid all the way across,
  otherwise the player and enemies fall off the right of the map.

Editing
-------
Just edit the text and relaunch the game. To regenerate the defaults run:

    python tools/gen_stages.py

If a stageN.txt is missing, the game generates and saves one automatically, so
deleting a file is a safe way to get a fresh default back.
