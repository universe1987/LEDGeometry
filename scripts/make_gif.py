#!/usr/bin/env python3
"""
Generate an animated GIF from LEDGeometry debug output.
Requires: Pillow  (pip install Pillow)

Usage:
    cd src && bash setup.sh && ./a.exe | python ../scripts/make_gif.py
    ./a.exe | python ../scripts/make_gif.py --output preview.gif --effects 1 4
"""
import os
import sys
import argparse

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from visualize import parse_stream

try:
    from PIL import Image, ImageDraw
except ImportError:
    print('make_gif.py: Pillow is required — pip install Pillow', file=sys.stderr)
    sys.exit(1)


def render_frame(coords, rgb_values, size):
    img = Image.new('RGB', (size, size), (0, 0, 0))
    draw = ImageDraw.Draw(img)
    cx = cy = size / 2
    scale = size / 2 - size * 0.05
    radius = max(2, int(size * 0.013))
    for (x, y), (r, g, b) in zip(coords, rgb_values):
        px = cx + x * scale
        py = cy + y * scale
        draw.ellipse([px - radius, py - radius, px + radius, py + radius], fill=(r, g, b))
    return img


def main():
    ap = argparse.ArgumentParser(description='Generate animated GIF from LEDGeometry debug output')
    ap.add_argument('--output', default='docs/preview.gif')
    ap.add_argument('--effects', nargs='+', type=int, default=None,
                    help='Effect indices to include (default: all)')
    ap.add_argument('--max-frames', type=int, default=36,
                    help='Max frames per effect (default: 36)')
    ap.add_argument('--size', type=int, default=400,
                    help='Canvas size in pixels (default: 400)')
    ap.add_argument('--max-fps', type=int, default=12,
                    help='Max playback fps in the GIF (default: 12)')
    args = ap.parse_args()

    effects = parse_stream(sys.stdin)
    if not effects:
        print('make_gif.py: no effects parsed from stdin.', file=sys.stderr)
        sys.exit(1)

    selected = [e for e in effects
                if args.effects is None or e['index'] in args.effects]
    if not selected:
        print('make_gif.py: no matching effects.', file=sys.stderr)
        sys.exit(1)

    pil_frames = []
    durations = []
    for eff in selected:
        coords = eff['coords']
        fps = min(eff['fps'], args.max_fps)
        step = max(1, eff['fps'] // fps)
        delay_ms = max(20, 1000 // fps)
        taken = 0
        for fi, rgb_values in enumerate(eff['frames']):
            if fi % step != 0:
                continue
            pil_frames.append(render_frame(coords, rgb_values, args.size))
            durations.append(delay_ms)
            taken += 1
            if taken >= args.max_frames:
                break

    if not pil_frames:
        print('make_gif.py: no frames rendered.', file=sys.stderr)
        sys.exit(1)

    out_dir = os.path.dirname(args.output)
    if out_dir:
        os.makedirs(out_dir, exist_ok=True)

    palette_frames = [
        f.quantize(colors=256, method=Image.Quantize.FASTOCTREE, dither=Image.Dither.NONE)
        for f in pil_frames
    ]
    palette_frames[0].save(
        args.output,
        save_all=True,
        append_images=palette_frames[1:],
        duration=durations,
        loop=0,
        optimize=False,
    )
    size_kb = os.path.getsize(args.output) / 1024
    total_s = sum(durations) / 1000
    print(f'make_gif.py: wrote {args.output} '
          f'({len(pil_frames)} frames, {total_s:.1f}s, {size_kb:.0f} KB)',
          file=sys.stderr)


if __name__ == '__main__':
    main()
