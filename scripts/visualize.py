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
