# Visualizer

This repository provides the code to make a music visualizer using a Raspberry Pi Pico H.

## Set Up
### Using the Pico-SDK
1. Make a directory for the pico-sdk and clone the repository
2. Run `git submodule update --init` inside the pico-sdk directory
3. Copy the `pico_sdk_import.cmake` from the pico-sdk directory into the root of project folder
4. Make sure to include the following at the top of your root `CMakeLists.txt`
```cmake
# Initialize project and pico-sdk
include(pico_sdk_import.cmake)
project(visualize C CXX ASM)
pico_sdk_init()
```

### Setting up the visualizer
1. Configure the settings you are using in [ws2812_config.h](/src/utils/include/utils/ws2812_config.h). These contain all the configurations necessary for your LED strips, disregard all other macros in other header files. Those are for use in other files. __Note__ the `MAX_HEX` macro is used to control brightness of the LEDs and consequently the current draw. Be mindful of your power source and make sure that the total number of power for all LEDs among all strips is suitable for your supply.
2. Configure the color of the visualizer using the `color` variable in [visualizer_4_strip.c](/src/prototypes/visualizer_4_strip.c)
3. To build the project, make a `/build` directory in the root of your project. From there run `cmake .. && make` to generate the build files
4. After the build completes, drag the `visualize.uf2` file to your Pico in BOOTSEL mode
5. Visualize :)


## Components Used
- Microcontroller: Raspberry Pi Pico H
- Logic level shifter: SN74HCT125N [Datasheet](#https://www.ti.com/lit/ds/symlink/sn74hct125.pdf)
- Microphone chip: MAX9814
- LEDs: WS2812B

## Other Notes
Many prototypes were generated during development for quick testing of components. These are included in the [prototypes](/src/prototypes) folder in the repository. You can use those to check your components for debugging or general use.

The FFT is set to run for 4 LED strips and an FFT size of 128. You can configure these settings for your implementation in [fft_tools.h](/src/utils/include/utils/fft_tools.h)

Schematics for the prototypes and the final visualizer can be found in `/schematics`

## Libraries
### KissFFT
This project includes `KissFFT`, licensed under the [BSD License](https://github.com/mborgerding/kissfft/blob/master/COPYING).

Source code included in `lib/kissfft/`.

[KissFFT](https://github.com/mborgerding/kissfft) by Mark Borgerding

### Pico-SDK
This project uses the [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk), developed and maintained by the Raspberry Pi Foundation.  

The Pico SDK provides the low-level C/C++ libraries and build tools that make it possible to develop software for the RP2040 microcontroller, including support for GPIO, PIO, timers, ADC, and more.

