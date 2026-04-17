#ifndef WS2812_CONFIG_H
#define WS2812_CONFIG_H

#define IS_RGBW false

#define NUM_PIXELS 35
#define NUM_STRIPS 8

#define FRAME_PORTRAIT 0
#define FRAME_LANDSCAPE 1
#define FRAME_ORIENTATION 1

#define VISUALIZER_ROWS ((FRAME_ORIENTATION == 0) ? NUM_PIXELS : NUM_STRIPS)
#define VISUALIZER_COLS ((FRAME_ORIENTATION == 0) ? NUM_STRIPS : NUM_PIXELS)

#define MAX_HEX 0x14
#define FADE_STEPS 500

#define WS2812_PINS_4 {10, 11, 12, 13}
#define WS2812_PINS_8 {21, 20, 19, 18, 10, 11, 12, 13}

// Safeguards
#if FRAME_ORIENTATION == FRAME_PORTRAIT
#pragma message("Animation orientation: PORTRAIT")
#elif FRAME_ORIENTATION == FRAME_LANDSCAPE
#pragma message("Animation orientation: LANDSCAPE")
#else
#error "Invalid PORTRAIT_FRAME"
#endif

#endif
