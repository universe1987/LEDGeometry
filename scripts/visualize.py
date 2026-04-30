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
  <button class="sp" data-s="0.5">0.5&#215;</button>
  <button class="sp on" data-s="1">1&#215;</button>
  <button class="sp" data-s="2">2&#215;</button>
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
    ctx.arc(cx + x * scale, cy + y * scale, r, 0, 6.2832);
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


def generate_html(effects):
    return HTML_TEMPLATE.replace('__LEDGEOMETRY_DATA__', json.dumps(effects), 1)


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
