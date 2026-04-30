# LEDGeometry
Light effects using Arduino and WS2812B addressable LED strips.

## Demo
This demo showcases 7 light effects for a heart-shaped WS2812B LED strip controlled by an Arduino Nano. For more information, see `examples/heart/heart.ino`.

<details>

### 1. Mono-color
<img src="./images/1_mono_color.gif" width="300"/>

### 2. Signal transmission
<img src="./images/2_signal_transmission.gif" width="300"/>

### 3. Ripple
<img src="./images/3_ripple.gif" width="300"/>

### 4. Pulse
<img src="./images/4_pulse.gif" width="300"/>

### 5. Tide
<img src="./images/5_tide.gif" width="300"/>

### 6. Spiral
<img src="./images/6_spiral.gif" width="300"/>

### 7. Flame
<img src="./images/7_flame.gif" width="300"/>

</details>

## Instructions
To reproduce the above effects, follow these steps.

1. **Materials**
   - One [Arduino Nano](https://www.arduino.cc/en/pmwiki.php?n=Main/ArduinoBoardNano)
   - One [WS2812B LED strip](https://www.amazon.ca/gp/product/B07RBVKLPX)
   - One [Mini USB cable](https://www.amazon.ca/gp/product/B00016W6NC)
   - [Optional] [Jumper wires](https://www.amazon.ca/gp/product/B01ERPEMAC/)
   - [Optional] [Clip cables](https://www.amazon.ca/gp/product/B06XCJ5YLY)
   - [Optional] Clear tape

2. **Hook up**
   1. Fold the LED strip to form a two-sided light.
   2. Shape it (e.g. heart) and attach it to a window.
   3. Connect the LED strip data pin to pin 7 on the Arduino Nano, and power/ground lines to 5 V and GND.

3. **Light up**
   1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
   2. Open `examples/heart/heart.ino` in the Arduino IDE.
   3. Copy `src/` to `<Arduino Libraries directory>/LEDGeometry`.
   4. Upload the sketch to your Arduino Nano.

4. **Calibrate** (to match the effect to your physical LED layout)
   1. Turn the LEDs on and take a photo.
   2. Run `scripts/locate_lights.py` to extract each LED's position from the photo and generate coordinate arrays.
   3. Copy the generated coordinate arrays into your `.ino` sketch.
   4. Upload the updated sketch to your Arduino Nano.

## Debug mode (desktop preview)

The library can be compiled and run on a desktop to preview effects without hardware.

**Requirements:** a C++11 compiler and the `lib8tion` library in a sibling directory (`../lib8tion`).

### Build and run

```bash
cd extras/debug
bash setup.sh                                    # compile with -DDEBUG → a.exe
./a.exe | python ../../scripts/visualize.py      # open animated preview in browser
```

The binary runs one full `loop()` pass and exits. `visualize.py` reads its output, writes a self-contained HTML file, and opens it in your default browser.

### Browser viewer

<img src="./docs/preview.gif" width="400"/>

The viewer shows all effects in tabs with full playback controls:

| Control | Behaviour |
|---|---|
| Effect tabs | Switch effect; playback resets to frame 0 |
| ▶ / ⏸ | Play / pause |
| Scrub bar | Jump to any frame |
| 0.5× 1× 2× 4× | Playback speed |
| ↻ Loop | Toggle looping |
| Frame counter | Current / total frames |

Custom output path:

```bash
./a.exe | python ../../scripts/visualize.py --output /path/to/preview.html
```

### Generate an animated GIF

```bash
./a.exe | python ../../scripts/make_gif.py                            # all effects → docs/preview.gif
./a.exe | python ../../scripts/make_gif.py --effects 1 4              # ripple + flame only
./a.exe | python ../../scripts/make_gif.py --output out.gif --max-frames 60 --size 600
```

Requires [Pillow](https://pillow.readthedocs.io/) (`pip install Pillow`).

### Debug output format

```
Effect 0 fps=10
Coords x0,y0 x1,y1 ... xN,yN
Frame 0 (r,g,b) (r,g,b) ... (r,g,b)
Frame 1 (r,g,b) ...
...
Effect 1 fps=20
Coords ...
Frame 0 ...
```

- **`Effect N fps=F`** — start of a new effect; `N` is zero-based, `F` is the frame rate.
- **`Coords x,y ...`** — one normalised `float,float` pair per LED in index order. For folded curves the mirrored second half is included.
- **`Frame N (r,g,b)...`** — one RGB tuple per LED for frame `N`.

To use debug mode in your own sketch, compile with `-DDEBUG` and include the same headers as `extras/debug/test.cpp`. No hardware or Arduino IDE is required.
