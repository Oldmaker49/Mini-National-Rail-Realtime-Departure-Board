// WAVESHARE_3-5inch_RP2040_ILI9488.h
// Works with Waveshare "Pico-ResTouch-LCD-3.5" HAT (SKU: 19907)

//#define RP2040_PIO_SPI
#define RPI_DISPLAY_TYPE

#define ILI9488_DRIVER     // WARNING: Do not connect ILI9488 display SDO to MISO if other devices share the SPI bus (TFT SDO does NOT tristate when CS is high)
#define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF

#define TFT_MISO  12  //0
#define TFT_MOSI  11  //3
#define TFT_SCLK  10  //2
#define TFT_CS     9  //20  // Chip select control pin
#define TFT_DC     8  //8   // Data Command control pin
#define TFT_RST   15  //19  // Reset pin (could connect to Arduino RESET pin)
#define TFT_BL    13  //       LED back-light

#define TOUCH_CS 16     // Chip select pin (T_CS) of touch screen *** DEFINED BUT TOUCH FUNCTIONS NOT NOT USED ***

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT

// ***GFXFF FONTS NOT USED SO DON'T LOAD THEM ***
//#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

#define SMOOTH_FONT

#define TFT_SPI_PORT 1 // Set to 0 if SPI0 pins are used, or 1 if spi1 pins used

// #define SPI_FREQUENCY   1000000
// #define SPI_FREQUENCY   5000000
// #define SPI_FREQUENCY  10000000
#define SPI_FREQUENCY  20000000
// #define SPI_FREQUENCY  27000000
// #define SPI_FREQUENCY  32000000
// #define SPI_FREQUENCY  40000000
//#define SPI_FREQUENCY  70000000

// Optional reduced SPI frequency for reading TFT
#define SPI_READ_FREQUENCY  20000000

// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY  2500000