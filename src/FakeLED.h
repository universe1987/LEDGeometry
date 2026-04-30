#pragma once
#include <stdint.h>

#include <iostream>

struct CRGB {
    uint8_t r;
    uint8_t g;
    uint8_t b;

    CRGB() : r(0), g(0), b(0) {}
    CRGB(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}

    CRGB(int color_code)
        : r((color_code >> 16) & 255),
          g((color_code >> 8) & 255),
          b(color_code & 255) {}

    CRGB& from_color_code(int color_code) {
        r = (color_code >> 16) & 255;
        g = (color_code >> 8) & 255;
        b = color_code & 255;
        return *this;
    }

    inline CRGB& from_hsv(uint8_t h, uint8_t s, uint8_t v) {
        uint8_t region, remainder, p, q, t;
        if (s == 0) {
            r = g = b = v;
            return *this;
        }
        region = h / 43;
        remainder = (h - region * 43) * 6;
        p = (v * (255 - s)) >> 8;
        q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;
        switch (region) {
            case 0:
                r = v;
                g = t;
                b = p;
                break;
            case 1:
                r = q;
                g = v;
                b = p;
                break;
            case 2:
                r = p;
                g = v;
                b = t;
                break;
            case 3:
                r = p;
                g = q;
                b = v;
                break;
            case 4:
                r = t;
                g = p;
                b = v;
                break;
            default:
                r = v;
                g = p;
                b = q;
                break;
        }
        return *this;
    }

    inline CRGB& setHue(uint8_t hue) {
        from_hsv(hue, 255, 255);
        return *this;
    }

    // Defined after lib8tion is included (needs nscale8x3_video).
    inline CRGB& fadeLightBy(uint8_t fade_factor);

    typedef enum {
        Aqua = 0x00FFFF,
        Black = 0x000000,
        Blue = 0x0000FF,
        Green = 0x008000,
        Lime = 0x00FF00,
        Red = 0xFF0000,
        White = 0xFFFFFF
    } HTMLColorCode;
};

struct CRGBPalette16 {
    CRGB entries[16];
    CRGBPalette16() {
        for (int i = 0; i < 16; i++) entries[i] = CRGB(0, 0, 0);
    }
    // Interpolates 4 anchor colours across 16 slots (anchors at 0, 5, 10, 15).
    CRGBPalette16(CRGB c1, CRGB c2, CRGB c3, CRGB c4) {
        const CRGB stops[4] = {c1, c2, c3, c4};
        for (int i = 0; i < 16; i++) {
            int seg = i < 5 ? 0 : i < 10 ? 1 : 2;
            int t = i - seg * 5;
            entries[i] = CRGB(
                (uint8_t)((int)stops[seg].r + ((int)stops[seg+1].r - (int)stops[seg].r) * t / 5),
                (uint8_t)((int)stops[seg].g + ((int)stops[seg+1].g - (int)stops[seg].g) * t / 5),
                (uint8_t)((int)stops[seg].b + ((int)stops[seg+1].b - (int)stops[seg].b) * t / 5)
            );
        }
    }
};

struct FastLEDType {
    long clock;

    FastLEDType() : clock(0) {}
    void show() {}
    void delay(unsigned long ms) { clock += ms; }
};

extern const CRGBPalette16 HeatColors_p;
extern FastLEDType FastLED;

// Wrap lib8tion in an anonymous namespace so that rand16seed (a non-static
// global defined in lib8tion/random8.h) gets per-TU internal linkage instead
// of becoming a duplicate global symbol across translation units.
// get_millisecond_timer is defined inside the block because lib8tion.h
// declares it in this scope and beat88/beatsin88 call it from here.
namespace {
#include "lib8tion.h"
uint32_t get_millisecond_timer() { return (uint32_t)::FastLED.clock; }
}  // anonymous namespace

// Now that nscale8x3_video is visible, define fadeLightBy.
inline CRGB& CRGB::fadeLightBy(uint8_t fade_factor) {
    nscale8x3_video(r, g, b, 255 - fade_factor);
    return *this;
}

inline CRGB blend(const CRGB& p1, const CRGB& p2, uint8_t amountOfP2) {
    uint8_t r = blend8(p1.r, p2.r, amountOfP2);
    uint8_t g = blend8(p1.g, p2.g, amountOfP2);
    uint8_t b = blend8(p1.b, p2.b, amountOfP2);
    return CRGB(r, g, b);
}

inline CRGB ColorFromPalette(const CRGBPalette16& pal, uint8_t index) {
    return pal.entries[index >> 4];
}
