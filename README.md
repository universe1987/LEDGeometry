# LEDGeometry
Light Effects using Arduino

## Demo
This demo show case 7 light effects for a heart-shaped WS2812b LED light controlled by Arduino nano. For more information, please see `example/heart/heart.ino`

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
To reproduce the above effect, please follow these steps.

1. Materials
     * One [Arduino Nano](https://www.arduino.cc/en/pmwiki.php?n=Main/ArduinoBoardNano)
     * One [WS2812b light](https://www.amazon.ca/gp/product/B07RBVKLPX)
     * One [Mini USB cable](https://www.amazon.ca/gp/product/B00016W6NC) for charging
     * [Optional] some [jumper wires](https://www.amazon.ca/gp/product/B01ERPEMAC/)
     * [Optional] some [clip cables](https://www.amazon.ca/gp/product/B06XCJ5YLY)
     * [Optional] clear tapes
2. Hook up
   1. Fold the LED light to form a two-sided light
   2. Make the LED light into a heart shape and attach it to a window
   3. [TODO] add wiring instructions
3. Light up
   1. Install the [Arduino IDE](https://www.arduino.cc/en/software)
   2. Open `example\heart\heart.ino` using Arduino IDE
   3. Copy `src/` to `<Arduino Libraries directory>/LEDGeometry`
   4. Upload the code to your Arduino board
4. Calibrate
   1. Take a photo while the lights are on.
   2. Use `scripts/locate_light.py` to get the locations of each light. [TODO] add details
   3. Copy-paste the coordinates of the lights into `examples\heart\heart.ino`
   4. Upload the code again to your Arduino board
