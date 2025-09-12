#ifndef WS2812_CONFIG_H
#define WS2812_CONFIG_H

#define IS_RGBW false
#define NUM_PIXELS 35
#define NUM_STRIPS 2
#define MAX_HEX 0x14
#define FADE_STEPS 500

// Simulated FFT energy data for 20 frames, two strips, 0–35 range
uint8_t band_energies[20][2] = {
    {  3,  2 },  // frame 0: very quiet
    { 10,  5 },  // frame 1: low picks up
    { 15,  8 },  // frame 2
    { 20, 15 },  // frame 3: mid grows too
    { 28, 18 },  // frame 4: low peaks
    { 35, 22 },  // frame 5: max bass hit
    { 30, 25 },  // frame 6: still loud
    { 25, 20 },  // frame 7: falling
    { 18, 18 },  // frame 8
    { 12, 22 },  // frame 9: mids are louder
    {  8, 30 },  // frame 10: mid peak
    {  5, 35 },  // frame 11: max mid hit
    {  8, 28 },  // frame 12: mid falling
    { 15, 22 },  // frame 13
    { 20, 18 },  // frame 14
    { 25, 10 },  // frame 15: lows build again
    { 30,  5 },  // frame 16
    { 35,  3 },  // frame 17: strong low again
    { 20,  0 },  // frame 18: decay
    {  0,  0 }   // frame 19: silence
};

#endif
