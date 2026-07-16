/**
 * @file User_Setup.h
 * @brief TFT_eSPI configuration for the ventilator display
 * 
 * This file configures the TFT_eSPI library for the specific display hardware.
 * Adjust pins and settings according to your TFT display.
 */

#ifndef _USERSETUP_H_
#define _USERSETUP_H_

// Display driver - uncomment the one you're using
#define ILI9341_DRIVER
//#define ST7735_DRIVER
//#define ILI9163_DRIVER
//#define S6D02A1_DRIVER
//#define R61581_DRIVER

// Display resolution
#define TFT_WIDTH  320
#define TFT_HEIGHT 240

// ESP32 pins (adjust according to your wiring)
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4

// SPI frequency
#define SPI_FREQUENCY  40000000

// Touch configuration (if touch is available)
//#define TOUCH_CS 21

// Font settings
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#endif // _USERSETUP_H_
