# LED Visualizer Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a Python script that reads the C++ debug binary's stdout, buffers one full animation loop, and opens a self-contained HTML file in the browser showing all effects with playback controls.

**Architecture:** The C++ binary emits an `Effect N fps=F` + `Coords x,y ...` header before each effect's frames, then exits after one `loop()` call in DEBUG mode. The Python script parses stdin into a list of effect objects, embeds them as JSON in an HTML template, writes the file, and opens it with `webbrowser.open()`. The browser renders LEDs on a `<canvas>` with tabs, scrub bar, speed buttons, and loop toggle — no server needed.

**Tech Stack:** C++11 (existing), Python 3 stdlib only (`sys`, `re`, `json`, `webbrowser`, `argparse`, `tempfile`), vanilla HTML/CSS/JS.

---

## File Map

| File | Action | Responsibility |
|---|---|---|
| `src/LEDCurve.h` | Modify | Add `effect_index` private member |
| `src/LEDCurve.cpp` | Modify | Emit `Effect N fps=F` + `Coords` header at start of `set_effect` under `#ifdef DEBUG`; init `effect_index` in ctor |
| `src/test.cpp` | Modify | Call `loop()` once under `#ifdef DEBUG` so binary exits after one pass |
| `scripts/visualize.py` | Create | stdin parser + self-contained HTML generator + browser launcher |

---

## Task 1: C++ Protocol Changes

**Files:**
- Modify: `src/LEDCurve.h`
- Modify: `src/LEDCurve.cpp`
- Modify: `src/test.cpp`

- [ ] **Step 1.1 — Add `effect_index` to `LEDCurve.h`**

Open `src/LEDCurve.h`. The current private section is:
```cpp
   private:
    bool folded;
    int frame_index;
    // Handle folded curve and display the assigned color of the LEDs.
    void display(int sleep_ms);
```
Change it to:
```cpp
   private:
    bool folded;
    int frame_index;
    int effect_index;
    // Handle folded curve and display the assigned color of the LEDs.
    void display(int sleep_ms);
```

- [ ] **Step 1.2 — Initialise `effect_index` in the constructor (`src/LEDCurve.cpp`)**

Current constructor init list (lines 13–19):
```cpp
    : leds(leds),
      shape(shape),
      color_scheduler(color_scheduler),
      blackout(nullptr),
      n_blackouts(0),
      folded(folded),
      frame_index(0){};
```
Change to:
```cpp
    : leds(leds),
      shape(shape),
      color_scheduler(color_scheduler),
      blackout(nullptr),
      n_blackouts(0),
      folded(folded),
      frame_index(0),
      effect_index(0){};
```

- [ ] **Step 1.3 — Emit `Effect` + `Coords` header in `set_effect` (`src/LEDCurve.cpp`)**

Current `set_effect` (lines 49–57):
```cpp
void LEDCurve::set_effect(LightEffect* effect, int fps, int n_seconds) {
    frame_index = 0;
    int sleep_ms = 1000 / fps;
    int n_iters = n_seconds * fps;
    for (int i = 0; i < n_iters; i++) {
        effect->update(this);
        display(sleep_ms);
    }
}
```
Replace with:
```cpp
void LEDCurve::set_effect(LightEffect* effect, int fps, int n_seconds) {
#ifdef DEBUG
    int n = shape->n_points();
    std::cout << "Effect " << effect_index << " fps=" << fps << "\n";
    std::cout << "Coords";
    for (int i = 0; i < n; i++)
        std::cout << " " << shape->x(i) << "," << shape->y(i);
    if (folded)
        for (int i = n - 1; i >= 0; i--)
            std::cout << " " << shape->x(i) << "," << shape->y(i);
    std::cout << "\n";
    effect_index++;
#endif
    frame_index = 0;
    int sleep_ms = 1000 / fps;
    int n_iters = n_seconds * fps;
    for (int i = 0; i < n_iters; i++) {
        effect->update(this);
        display(sleep_ms);
    }
}
```

The `Coords` line emits `n` positions for non-folded curves and `2n` for folded (mirrored second half in reverse), matching the LED indices in every `Frame` line.

- [ ] **Step 1.4 — Make `main()` call `loop()` once under `#ifdef DEBUG` (`src/test.cpp`)**

Current `main()` (lines 97–102):
```cpp
int main() {
    setup();
    while (true) {
        loop();
    }
}
```
Replace with:
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

- [ ] **Step 1.5 — Rebuild and verify output format**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry/src
bash setup.sh && ./a.exe 2>/dev/null | head -4
```

Expected (exact numbers will vary):
```
Effect 0 fps=10
Coords 0.0771,0.667 0.1752,0.6139 ...
Frame 0 (45,255,0) (45,255,0) ...
Frame 1 (47,253,2) ...
```

- [ ] **Step 1.6 — Commit**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry
git add src/LEDCurve.h src/LEDCurve.cpp src/test.cpp
git commit -m "feat: emit Effect/Coords protocol headers in DEBUG mode"
```

---

## Task 2: Python stdin Parser

**Files:**
- Create: `scripts/visualize.py` (parser functions only; HTML generation added in Task 3)

- [ ] **Step 2.1 — Create `scripts/visualize.py` with `parse_stream()`**

Create `scripts/visualize.py`:

```python
#!/usr/bin/env python3
"""
LEDGeometry Visualizer

Reads the LEDGeometry debug binary output from stdin, buffers one full
loop() pass, and opens a self-contained HTML animation in the browser.

Usage:
    cd src && bash setup.sh && ./a.exe | python ../scripts/visualize.py
    ./a.exe | python ../scripts/visualize.py --output preview.html
"""

import re
import sys
import json
import argparse
import os
import tempfile
import webbrowser

_FRAME_RE = re.compile(r'\((\d+),(\d+),(\d+)\)')


def parse_stream(lines):
    """Parse LEDGeometry debug output into a list of effect dicts.

    Each dict: {'index': int, 'fps': int, 'coords': [[x,y],...], 'frames': [[[r,g,b],...], ...]}
    """
    effects = []
    current = None
    for line in lines:
        line = line.rstrip('\n')
        if line.startswith('Effect '):
            parts = line.split()
            idx = int(parts[1])
            fps = 30
            for part in parts[2:]:
                if part.startswith('fps='):
                    fps = int(part[4:])
            current = {'index': idx, 'fps': fps, 'coords': [], 'frames': []}
            effects.append(current)
        elif line.startswith('Coords ') and current is not None:
            pairs = line[7:].split()
            current['coords'] = [[float(v) for v in p.split(',')] for p in pairs]
        elif line.startswith('Frame ') and current is not None:
            rgbs = [[int(r), int(g), int(b)]
                    for r, g, b in _FRAME_RE.findall(line)]
            current['frames'].append(rgbs)
    return effects
```

- [ ] **Step 2.2 — Verify the parser with an inline self-test**

Add a `__main__` block at the bottom of `scripts/visualize.py` that runs a quick self-test when called without stdin data (for development only — will be replaced in Task 3):

```python
if __name__ == '__main__':
    _sample = [
        'Effect 0 fps=10',
        'Coords 0.5,0.6 -0.5,0.6 0.0,-0.7',
        'Frame 0 (255,0,0) (0,255,0) (0,0,255)',
        'Frame 1 (128,0,0) (0,128,0) (0,0,128)',
        'Effect 1 fps=20',
        'Coords 0.5,0.6 -0.5,0.6 0.0,-0.7',
        'Frame 0 (10,20,30) (40,50,60) (70,80,90)',
    ]
    effects = parse_stream(_sample)
    assert len(effects) == 2, f'expected 2 effects, got {len(effects)}'
    assert effects[0]['fps'] == 10
    assert effects[0]['coords'][0] == [0.5, 0.6]
    assert effects[0]['frames'][1][2] == [0, 0, 128]
    assert effects[1]['index'] == 1
    assert effects[1]['fps'] == 20
    print('parse_stream: OK')
```

- [ ] **Step 2.3 — Run the self-test**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry
python scripts/visualize.py
```

Expected output:
```
parse_stream: OK
```

- [ ] **Step 2.4 — Commit**

```bash
git add scripts/visualize.py
git commit -m "feat: add LEDGeometry visualizer stdin parser"
```

---

## Task 3: HTML Template and Generator

**Files:**
- Modify: `scripts/visualize.py` — add `HTML_TEMPLATE`, `generate_html()`, `main()`

Replace the `if __name__ == '__main__':` block from Task 2 with the full implementation below. The complete final `scripts/visualize.py` is:

- [ ] **Step 3.1 — Add `HTML_TEMPLATE` constant**

Insert after the `parse_stream` function (before `if __name__`):

```python
HTML_TEMPLATE = """<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>LEDGeometry Preview</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#111;color:#eee;font-family:monospace;display:flex;flex-direction:column;height:100vh;overflow:hidden}
#tabs{display:flex;gap:4px;padding:8px 10px;background:#1a1a1a;flex-shrink:0;flex-wrap:wrap}
.tab{padding:5px 14px;border-radius:4px;cursor:pointer;background:#333;border:none;color:#aaa;font:inherit;font-size:13px}
.tab.active{background:#4a9eff;color:#fff}
#canvas-wrap{flex:1;display:flex;align-items:center;justify-content:center;background:#000;min-height:0}
canvas{display:block}
#controls{display:flex;align-items:center;gap:10px;padding:8px 12px;background:#1a1a1a;flex-shrink:0;flex-wrap:wrap}
#scrub{flex:1;min-width:120px;accent-color:#4a9eff}
.sp{padding:3px 8px;border-radius:3px;cursor:pointer;background:#333;border:1px solid #555;color:#ccc;font:inherit;font-size:12px}
.sp.on{background:#4a9eff;color:#fff;border-color:#4a9eff}
#play{padding:4px 14px;cursor:pointer;background:#4a9eff;border:none;border-radius:4px;color:#fff;font:inherit;font-size:15px}
#loop{padding:3px 8px;border-radius:3px;cursor:pointer;background:#333;border:1px solid #555;color:#ccc;font:inherit;font-size:12px}
#loop.on{background:#555;color:#fff;border-color:#888}
#ctr{min-width:80px;text-align:right;color:#888;font-size:12px}
</style>
</head>
<body>
<div id="tabs"></div>
<div id="canvas-wrap"><canvas id="c"></canvas></div>
<div id="controls">
  <button id="play">&#9654;</button>
  <input id="scrub" type="range" min="0" value="0">
  <button class="sp on" data-s="0.5">0.5&#215;</button>
  <button class="sp" data-s="1">1&#215;</button>
  <button class="sp on" data-s="2">2&#215;</button>
  <button class="sp" data-s="4">4&#215;</button>
  <button id="loop" class="on">&#8635; Loop</button>
  <span id="ctr">0 / 0</span>
</div>
<script>
const DATA = __LEDGEOMETRY_DATA__;

const canvas = document.getElementById('c');
const ctx = canvas.getContext('2d');
const tabsEl = document.getElementById('tabs');
const scrub = document.getElementById('scrub');
const playBtn = document.getElementById('play');
const loopBtn = document.getElementById('loop');
const ctr = document.getElementById('ctr');

// Activate the 1x speed button on load
document.querySelectorAll('.sp').forEach(b => b.classList.toggle('on', b.dataset.s === '1'));

let S = {ei: 0, fi: 0, playing: true, speed: 1, loop: true};
let lastTs = null, accum = 0;

DATA.forEach((eff, i) => {
  const b = document.createElement('button');
  b.className = 'tab' + (i === 0 ? ' active' : '');
  b.textContent = 'Effect ' + i;
  b.onclick = () => switchEffect(i);
  tabsEl.appendChild(b);
});

function switchEffect(i) {
  S.ei = i; S.fi = 0; accum = 0; lastTs = null;
  scrub.max = DATA[i].frames.length - 1;
  scrub.value = 0;
  tabsEl.querySelectorAll('.tab').forEach((b, j) => b.classList.toggle('active', j === i));
  draw();
}

function draw() {
  const eff = DATA[S.ei];
  const frame = eff.frames[S.fi];
  const coords = eff.coords;
  const wrap = canvas.parentElement;
  const W = wrap.clientWidth, H = wrap.clientHeight;
  if (canvas.width !== W) canvas.width = W;
  if (canvas.height !== H) canvas.height = H;
  const margin = Math.min(W, H) * 0.05;
  const scale = (Math.min(W, H) / 2) - margin;
  const cx = W / 2, cy = H / 2;
  const r = Math.max(2, Math.min(W, H) * 0.012);
  ctx.fillStyle = '#000';
  ctx.fillRect(0, 0, W, H);
  for (let i = 0; i < coords.length; i++) {
    const [x, y] = coords[i];
    const [rv, gv, bv] = frame[i];
    ctx.fillStyle = 'rgb(' + rv + ',' + gv + ',' + bv + ')';
    ctx.beginPath();
    ctx.arc(cx + x * scale, cy - y * scale, r, 0, 6.2832);
    ctx.fill();
  }
  ctr.textContent = S.fi + ' / ' + (eff.frames.length - 1);
  scrub.value = S.fi;
}

function tick(ts) {
  if (S.playing && lastTs !== null) {
    accum += (ts - lastTs) * S.speed;
    const eff = DATA[S.ei];
    const ms = 1000 / eff.fps;
    while (accum >= ms) {
      accum -= ms;
      S.fi++;
      if (S.fi >= eff.frames.length) {
        if (S.loop) { S.fi = 0; }
        else { S.fi = eff.frames.length - 1; S.playing = false; playBtn.innerHTML = '&#9654;'; break; }
      }
    }
    draw();
  }
  lastTs = S.playing ? ts : null;
  requestAnimationFrame(tick);
}

playBtn.onclick = () => {
  S.playing = !S.playing;
  playBtn.innerHTML = S.playing ? '&#9646;&#9646;' : '&#9654;';
  if (S.playing) lastTs = null;
};
loopBtn.onclick = () => { S.loop = !S.loop; loopBtn.classList.toggle('on', S.loop); };
scrub.oninput = () => { S.fi = +scrub.value; draw(); };
document.querySelectorAll('.sp').forEach(b => {
  b.onclick = () => {
    S.speed = parseFloat(b.dataset.s);
    document.querySelectorAll('.sp').forEach(x => x.classList.toggle('on', x === b));
  };
});
window.addEventListener('resize', draw);

switchEffect(0);
playBtn.innerHTML = '&#9646;&#9646;';
requestAnimationFrame(tick);
</script>
</body>
</html>"""
```

- [ ] **Step 3.2 — Add `generate_html()` and `main()`**

Replace the `if __name__ == '__main__':` block with:

```python
def generate_html(effects):
    return HTML_TEMPLATE.replace('__LEDGEOMETRY_DATA__', json.dumps(effects))


def main():
    parser = argparse.ArgumentParser(description='LEDGeometry visualizer')
    parser.add_argument('--output', default=None,
                        help='Write HTML to this path instead of a temp file')
    args = parser.parse_args()

    effects = parse_stream(sys.stdin)
    if not effects:
        print('visualize.py: no effects parsed from stdin.', file=sys.stderr)
        sys.exit(1)

    html = generate_html(effects)

    if args.output:
        path = os.path.abspath(args.output)
    else:
        fd, path = tempfile.mkstemp(suffix='.html', prefix='led_preview_')
        os.close(fd)

    with open(path, 'w') as f:
        f.write(html)

    print(f'visualize.py: wrote {path}', file=sys.stderr)
    webbrowser.open('file://' + path)


if __name__ == '__main__':
    main()
```

- [ ] **Step 3.3 — Commit**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry
git add scripts/visualize.py
git commit -m "feat: add HTML generator and browser launcher to visualizer"
```

---

## Task 4: End-to-End Verification

- [ ] **Step 4.1 — Run the full pipeline**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry/src
bash setup.sh && ./a.exe | python ../scripts/visualize.py
```

Expected terminal output (stderr):
```
visualize.py: wrote /tmp/led_preview_XXXXXX.html
```

A browser window should open automatically.

- [ ] **Step 4.2 — Verify in the browser**

Check each of the following:

| Item | Expected |
|---|---|
| Effect tabs | One tab per `set_effect` call in `test.cpp` (5–6 tabs for the pumpkin example) |
| Canvas | Black background, colored dots arranged in the pumpkin/heart shape |
| Blackout LEDs | Show as black dots `(0,0,0)` at their positions |
| Animation | Dots change color smoothly as frames advance |
| Scrub bar | Dragging jumps to that frame and pauses |
| Speed buttons | 0.5× slows, 4× speeds up animation noticeably |
| Loop toggle | Turning off stops at last frame; turning on resumes looping |
| Effect tabs | Clicking a tab switches shape/animation and resets to frame 0 |

- [ ] **Step 4.3 — Test `--output` flag**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry/src
./a.exe | python ../scripts/visualize.py --output /tmp/my_preview.html
ls -lh /tmp/my_preview.html
```

Expected: file exists, size > 100 KB (embedded frame data).

- [ ] **Step 4.4 — Add `.superpowers/` to `.gitignore`**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry
echo '.superpowers/' >> .gitignore
git add .gitignore
git commit -m "chore: ignore .superpowers brainstorm session files"
```

- [ ] **Step 4.5 — Final commit**

```bash
cd /Users/Shared/F_Disk/Data/Code/LEDGeometry
git add -A
git status  # verify nothing unexpected is staged
git commit -m "feat: complete LED visualizer — HTML/JS playback frontend"
```

---

## Self-Review

**Spec coverage:**
- ✅ `Effect N fps=F` + `Coords` emitted before each effect's frames
- ✅ `test.cpp` exits after one `loop()` call under `#ifdef DEBUG`
- ✅ Python parses stdin into effect objects with coords, fps, frames
- ✅ HTML embeds all data inline — no server needed
- ✅ Effect tabs, play/pause, scrub bar, speed buttons, loop toggle, frame counter
- ✅ Canvas renders flat circles; y-axis flipped; radius ~1% of shorter dimension
- ✅ `--output` flag for custom path
- ✅ `webbrowser.open()` launches browser
- ✅ stdlib only — no extra Python packages

**Placeholder scan:** No TBDs, TODOs, or vague steps — all code is complete.

**Type consistency:**
- `parse_stream()` returns `list[dict]` with keys `index`, `fps`, `coords`, `frames` — used consistently in `generate_html()` and `HTML_TEMPLATE` (`DATA[S.ei].fps`, `DATA[S.ei].frames`, `DATA[S.ei].coords`)
- `__LEDGEOMETRY_DATA__` sentinel used in both `HTML_TEMPLATE` and `generate_html()` ✅
