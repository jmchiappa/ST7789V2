#ifndef __ST7735V2__
#define __ST7735V2__

#include "Adafruit_ST7735.h"
#include "Adafruit_SPITFT.h"
#include "Adafruit_GFX.h"
#include "glcdfont.c"
#include "SPI.h"

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
#ifdef __AVR__
  return &(((GFXglyph *)pgm_read_pointer(&gfxFont->glyph))[c]);
#else
  // expression in __AVR__ section may generate "dereferencing type-punned
  // pointer will break strict-aliasing rules" warning In fact, on other
  // platforms (such as STM32) there is no need to do this pointer magic as
  // program memory may be read in a usual way So expression may be simplified
  return gfxFont->glyph + c;
#endif //__AVR__
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
#ifdef __AVR__
  return (uint8_t *)pgm_read_pointer(&gfxFont->bitmap);
#else
  // expression in __AVR__ section generates "dereferencing type-punned pointer
  // will break strict-aliasing rules" warning In fact, on other platforms (such
  // as STM32) there is no need to do this pointer magic as program memory may
  // be read in a usual way So expression may be simplified
  return gfxFont->bitmap;
#endif //__AVR__
}

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

class ST7735V2 : public Adafruit_ST7735 {
  public:
    ST7735V2(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst);

    void FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
    void FillScreenFast(uint16_t color);
    void drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h);

    uint16_t getWidth(void) { return Adafruit_ST7735::_width; }
    uint16_t getHeight(void) {return Adafruit_ST7735::_height; }

  void SPIsettings(SPISettings settings) {
    hwspi.settings = settings;
  }

  void SPI_CS_HIGH(void) {
    digitalWriteFast(cs_, HIGH);
  }

  void SPI_CS_LOW(void) {
    digitalWriteFast(cs_, LOW);
  }

  void SPI_DC_HIGH(void) {
    digitalWriteFast(dc_, HIGH);
  }

  void SPI_DC_LOW(void) {
    digitalWriteFast(dc_, LOW);
  }
  // using Adafruit_GFX::drawChar;
  void drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size_x,
                            uint8_t size_y);
  protected:
    using Print::write;
    size_t write(uint8_t);

    void startWrite() {
      hwspi._spi->beginTransaction(hwspi.settings);
      digitalWriteFast(cs_,LOW);
    }
    void endWrite() {
      digitalWriteFast(cs_,HIGH);
      hwspi._spi->endTransaction();
    }
    // void begin(uint32_t freq);
    // void ST7735_WriteCommand(uint8_t cmd);
    // void ST7735_WriteData(uint8_t* buff, size_t buff_size);
    // void setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
    // void ST7735_ExecuteCommandList(const uint8_t *addr);


    PinName cs_ = NC,
            dc_ = NC,
            reset_ = NC;


};

#endif