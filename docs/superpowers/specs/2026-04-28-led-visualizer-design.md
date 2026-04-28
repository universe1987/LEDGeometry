# LED Visualizer — Design Spec
Date: 2026-04-28

## Goal

Add a frontend to LEDGeometry that lets users preview light effects on a desktop without hardware. The C++ debug binary produces all data; a Python script buffers it and writes a self-contained HTML file that plays the animation in a browser.

---

## Data Protocol (C++ → Python)

### Changes to `src/LEDCurve.cpp`

Under `#ifdef DEBUG`, `LEDCurve::set_effect()` emits a two-line header **before** starting the frame loop:

```
Effect N
Coords x0,y0 x1,y1 ... xK,yK
Frame 0 (r,g,b) (r,g,b) ...
Frame 1 (r,g,b) ...
...
```

**`Effect N`** — zero-based integer, increments each time `set_effect` is called.

**`Coords x,y ...`** — space-separated `float,float` pairs, one per LED, in LED index order. For folded curves the mirrored second half is included, so the count matches the frame data exactly. Values are normalised to `[-1, 1]`.

Frame lines are unchanged from the existing debug output.

### Change to `src/test.cpp`

`main()` calls `loop()` exactly once under `#ifdef DEBUG` then returns, so the binary exits and closes stdout naturally:

```cpp
int main() {
    setup();
#ifdef DEBUG
    loop();
#else
    while (true) { loop(); }
#endif
}
```

---

## Python Script (`scripts/visualize.py`)

### Invocation

```bash
cd src && bash setup.sh && ./a.exe | python ../scripts/visualize.py
```

Optional flag: `--output <path>` overrides the default output file (`/tmp/led_preview.html`).

### Behaviour

1. Read stdin line by line until EOF.
2. Parse into a list of effect objects:
   ```
   effects = [
     { "coords": [(x0,y0), ...], "frames": [[(r,g,b), ...], ...] },
     ...
   ]
   ```
3. Serialise `effects` as a JSON literal.
4. Write a self-contained HTML file (see below) with the JSON embedded.
5. Open the file with `webbrowser.open()`.

No external Python packages required — only stdlib (`sys`, `json`, `webbrowser`, `argparse`).

---

## HTML/JS Viewer

Single self-contained file. All data is in an inline `<script>` block; no network requests after opening.

### Layout

```
┌────────────────────────────────────────┐
│  [Effect 0] [Effect 1] [Effect 2] …   │  ← tab row
├────────────────────────────────────────┤
│                                        │
│         black canvas (LEDs)            │
│                                        │
├────────────────────────────────────────┤
│  ◀━━━━━━━━━━●━━━━━━━━━━━━━━━━━━━━━▶  │  ← scrub bar
│  ▶  0.5× 1× 2× 4×  [↻ Loop]  12/300  │  ← controls
└────────────────────────────────────────┘
```

### Canvas rendering

- Background: `#000000`
- Each LED: filled circle, colour taken directly from the frame's `(r,g,b)` tuple
- Position: normalised `(x, y)` mapped to canvas pixels; y-axis flipped (positive y = up)
- Radius: fixed at ~1 % of canvas shorter dimension, so dots don't overlap on typical layouts

### Controls

| Element | Behaviour |
|---|---|
| Effect tabs | Switch to that effect; playback resets to frame 0 |
| ▶ / ⏸ | Play / pause |
| Scrub bar | `<input type="range">`, drag to any frame in the current effect |
| Speed buttons | 0.5× 1× 2× 4× — scale the per-frame interval |
| Loop toggle | When on, effect repeats; when off, playback stops at last frame |
| Frame counter | `current / total` displayed as plain text |

### Playback timing

The frame interval defaults to `1000 / fps` ms where `fps` is inferred from the effect metadata (to be added to the `Effect` header line — see open question below). If fps is unavailable, default to 30 fps. Speed buttons multiply the interval.

---

## Open Questions / Future Work

- **FPS metadata**: The `Effect N` header could include the fps used by that `set_effect` call (e.g. `Effect 0 fps=20`), making playback timing exact. Not blocking for v1 — default 30 fps is acceptable.
- **Effect names**: Currently effects are labelled `Effect 0`, `Effect 1`, etc. A future change could emit a human-readable name.
- **Export**: The output HTML is already self-contained and shareable as-is.

---

## Files Changed / Created

| File | Change |
|---|---|
| `src/LEDCurve.cpp` | Emit `Effect N` + `Coords` header in `set_effect` under `#ifdef DEBUG` |
| `src/LEDCurve.h` | Add `effect_index` counter to private members |
| `src/test.cpp` | `main()` calls `loop()` once under `#ifdef DEBUG` |
| `scripts/visualize.py` | New — stdin parser + HTML generator |
