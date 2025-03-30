#include "ST7789V2.h"
// #include "Adafruit_ST7735.h"
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#define DELAY 0x80

#ifndef SPI_DEFAULT_FREQ
# define SPI_DEFAULT_FREQ 32000000 ///< Default SPI data clock frequency
#endif

/*!
    @brief  Instantiate ST7735 for STM32 driver with a dedicated hardware SPI
    @param  spiClass  Pointer to an SPI device to use (e.g. &SPI1)
    @param  cs   Chip select pin #
    @param  dc   Data/Command pin #
    @param  rst  Reset pin # (optional, pass -1 if unused)
*/

ST7789V2::ST7789V2(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst)
  : Adafruit_ST7789(spiClass,  cs,  dc,  rst ) {
      cs_ = digitalPinToPinName(cs);
      dc_ = digitalPinToPinName(dc);
      reset_ = digitalPinToPinName(rst);
  }

// void ST7789V2::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
//     // column address set
//     ST7735_WriteCommand(ST77XX_CASET);
//     uint8_t data[] = { 0x00, x0 + _colstart, 0x00, x1 + _colstart };
//     ST7735_WriteData(data, sizeof(data));

//     // row address set
//     ST7735_WriteCommand(ST77XX_RASET);
//     data[1] = y0 + _rowstart;
//     data[3] = y1 + _rowstart;
//     ST7735_WriteData(data, sizeof(data));

//     // write to RAM
//     ST7735_WriteCommand(ST77XX_RAMWR);
// }

void ST7789V2::FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    // if((x >= Adafruit_ST7735::_width) || (y >= Adafruit_ST7735::_height)) return;
    // if((x + w - 1) >= Adafruit_ST7735::_width) w = Adafruit_ST7735::_width - x;
    // if((y + h - 1) >= Adafruit_ST7735::_height) h = Adafruit_ST7735::_height - y;
    uint8_t nbChunks = 0;
    // uint8_t pixel[] = {color & 0xFF,color >> 8};
    uint8_t pixel[] = {color >> 8,color & 0xFF};

    uint32_t volume = h * w * sizeof(pixel);

    uint8_t *buffer = NULL;

    // SPI driver can't send more than 64KB
    while(volume>65536) {
      volume >>=1;
      nbChunks++;
    }

    do {
      buffer = (uint8_t *)malloc(volume);
      if(buffer == NULL) {
        volume >>= 1;
        nbChunks++;
      }
    }
    while (buffer == NULL );

    for(uint32_t p = 0; p < volume; p+=2)
      memcpy(buffer + p, pixel, sizeof(pixel));

    // on prend arbitrairement une variable pour découper le transfert
    uint16_t chunkBlockSize = h / (1 << nbChunks);
    uint16_t chunkNb = 0;
    uint8_t nbCycles = 1 << nbChunks;
    uint16_t y1;
    uint16_t hrest;
    do {
      hrest = h - chunkNb * chunkBlockSize;
      uint16_t chunkSize = min(chunkBlockSize, hrest);
      y1 = y + chunkNb * chunkBlockSize;
      startWrite();
      setAddrWindow(x, y1, x+w, y1 + chunkSize);

      // Prepare whole line in a single buffer
      // uint8_t *buffer = (uint8_t *)malloc(h * w * sizeof(pixel));

      // digitalWriteFast(dc_,HIGH);
      hwspi._spi->transfer( buffer, w * chunkSize * sizeof(pixel));
      endWrite();
    } while(chunkNb++ < nbCycles);

    free(buffer);
}

void ST7789V2::FillScreenFast(uint16_t color) {
    FillRectangleFast(0, 0, Adafruit_ST7789::_width, Adafruit_ST7789::_height, color);
}

void ST7789V2::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) {
  uint8_t nbChunks = 0;
  uint32_t volume = h * w * 2;

  for(uint32_t i=0;i<w*h;i++) {
    bitmap[i] = ((bitmap[i]>>8) & 0xFF) | ((bitmap[i] << 8) & 0xFF00);
  }
  uint8_t *buffer = (uint8_t *)bitmap;

  // SPI driver can't send more than 64KB
  while(volume>65536) {
    volume >>=1;
    nbChunks++;
  }

  // on prend arbitrairement une variable pour découper le transfert
  uint16_t chunkBlockSize = h / (1 << nbChunks);
  uint16_t chunkNb = 0;
  uint8_t nbCycles = 1 << nbChunks;
  uint16_t y1;
  uint16_t hrest;

  startWrite();
  setAddrWindow(x,y,w,h);

  do {
    hrest = h - chunkNb * chunkBlockSize;
    uint16_t chunkSize = min(chunkBlockSize, hrest);
    y1 = y + chunkNb * chunkBlockSize;
    startWrite();
    setAddrWindow(x, y1, x+w, y1 + chunkSize);

    hwspi._spi->transfer( &buffer[chunkNb * chunkBlockSize * 2 * w], w * chunkSize * 2);
    endWrite();
  } while(chunkNb++ < nbCycles);
}

size_t ST7789V2::write(uint8_t c) {
  if (!gfxFont) { // 'Classic' built-in font

    if (c == '\n') {              // Newline?
      cursor_x = 0;               // Reset x to zero,
      cursor_y += textsize_y * 8; // advance y one line
    } else if (c != '\r') {       // Ignore carriage returns
      if (wrap && ((cursor_x + textsize_x * 6) > _width)) { // Off right?
        cursor_x = 0;                                       // Reset x to zero,
        cursor_y += textsize_y * 8; // advance y one line
      }
      drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
               textsize_y);
      cursor_x += textsize_x * 6; // Advance x one char
    }

  } else { // Custom font

    if (c == '\n') {
      cursor_x = 0;
      cursor_y +=
          (int16_t)textsize_y * (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
    } else if (c != '\r') {
      uint8_t first = pgm_read_byte(&gfxFont->first);
      if ((c >= first) && (c <= (uint8_t)pgm_read_byte(&gfxFont->last))) {
        GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c - first);
        uint8_t w = pgm_read_byte(&glyph->width),
                h = pgm_read_byte(&glyph->height);
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = (int8_t)pgm_read_byte(&glyph->xOffset); // sic
          if (wrap && ((cursor_x + textsize_x * (xo + w)) > _width)) {
            cursor_x = 0;
            cursor_y += (int16_t)textsize_y *
                        (uint8_t)pgm_read_byte(&gfxFont->yAdvance);
          }
          drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize_x,
                   textsize_y);
        }
        cursor_x +=
            (uint8_t)pgm_read_byte(&glyph->xAdvance) * (int16_t)textsize_x;
      }
    }
  }
  return 1;
}

// Draw a character - enhanced version 
/**************************************************************************/
/*!
   @brief   Draw a single character
    @param    x   Bottom left corner x coordinate
    @param    y   Bottom left corner y coordinate
    @param    c   The 8-bit font-indexed character (likely ascii)
    @param    color 16-bit 5-6-5 Color to draw chraracter with
    @param    bg 16-bit 5-6-5 Color to fill background with (if same as color,
   no background)
    @param    size_x  Font magnification level in X-axis, 1 is 'original' size
    @param    size_y  Font magnification level in Y-axis, 1 is 'original' size
*/
/**************************************************************************/
void ST7789V2::drawChar(int16_t x, int16_t y, unsigned char c,
                            uint16_t color, uint16_t bg, uint8_t size_x,
                            uint8_t size_y) {

  if (!gfxFont) { // 'Classic' built-in font

    if ((x >= _width) ||              // Clip right
        (y >= _height) ||             // Clip bottom
        ((x + 6 * size_x - 1) < 0) || // Clip left
        ((y + 8 * size_y - 1) < 0))   // Clip top
      return;

    if (!_cp437 && (c >= 176))
      c++; // Handle 'classic' charset behavior

    startWrite();
    int8_t rp, rb; // Count sequential FG and BG pixels in column
    for (int8_t i = 0; i < 5; i++, rp = 0, rb = 0) { // Char bitmap = 5 columns
      uint8_t line = pgm_read_byte(&font[c * 5 + i]);
      for (int8_t j = 0; j < 8; j++, line >>= 1) {
        if (line & 1) {
          rp++;
          if (!(1 & (line >> 1)) ||
              j == 7) { // write sequential pixels with writeFillRect
            if (rp == 1 && size_x == 1 && size_y == 1) {
              writePixel(x + i, y + j, color);
            } else if (rp > 0) {
              writeFillRect(x + i * size_x, y + (j - (rp - 1)) * size_y, size_x,
                            size_y * rp, color);
            }
            rp = 0;
          }
        } else if (bg != color) {
          rb++;
          if (1 & (line >> 1) ||
              j == 7) { // write sequential pixels with writeFillRect
            if (rb == 1 && size_x == 1 && size_y == 1) {
              writePixel(x + i, y + j, bg);
            } else if (rb > 0) {
              writeFillRect(x + i * size_x, y + (j - (rb - 1)) * size_y, size_x,
                            size_y * rb, bg);
            }
            rb = 0;
          }
        }
      }
    }
    if (bg != color) { // If opaque, draw vertical line for last column
      if (size_x == 1 && size_y == 1)
        writeFastVLine(x + 5, y, 8, bg);
      else
        writeFillRect(x + 5 * size_x, y, size_x, 8 * size_y, bg);
    }
    endWrite();

  } else { // Custom font

    // Character is assumed previously filtered by write() to eliminate
    // newlines, returns, non-printable characters, etc.  Calling
    // drawChar() directly with 'bad' characters of font may cause mayhem!

    c -= (uint8_t)pgm_read_byte(&gfxFont->first);
    GFXglyph *glyph = pgm_read_glyph_ptr(gfxFont, c);
    uint8_t *bitmap = pgm_read_bitmap_ptr(gfxFont);

    uint16_t bo = pgm_read_word(&glyph->bitmapOffset);
    uint8_t w = pgm_read_byte(&glyph->width), h = pgm_read_byte(&glyph->height);
    int8_t xo = pgm_read_byte(&glyph->xOffset),
           yo = pgm_read_byte(&glyph->yOffset);
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;

    if (size_x > 1 || size_y > 1) {
      xo16 = xo;
      yo16 = yo;
    }

    // Todo: Add character clipping here

    // NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
    // THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
    // has typically been used with the 'classic' font to overwrite old
    // screen contents with new data.  This ONLY works because the
    // characters are a uniform size; it's not a sensible thing to do with
    // proportionally-spaced fonts with glyphs of varying sizes (and that
    // may overlap).  To replace previously-drawn text when using a custom
    // font, use the getTextBounds() function to determine the smallest
    // rectangle encompassing a string, erase the area with fillRect(),
    // then draw new text.  This WILL infortunately 'blink' the text, but
    // is unavoidable.  Drawing 'background' pixels will NOT fix this,
    // only creates a new set of problems.  Have an idea to work around
    // this (a canvas object type for MCUs that can afford the RAM and
    // displays supporting setAddrWindow() and pushColors()), but haven't
    // implemented this yet.

    startWrite();
    uint16_t w_ = w*size_x;
    uint16_t h_ = h*size_y;
    uint16_t len = w_ * h_;
    uint16_t *buffer = (uint16_t *)malloc( len * 2 );
    color = (color>>8) | ( color << 8);
    bg = (bg>>8) | (bg << 8);
    uint8_t bgPixels[] = { bg & 0xFF, bg>>8 };
    for(uint16_t i=0;i<len;i++)
      memcpy( &buffer[i], bgPixels, 2);
    
    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = pgm_read_byte(&bitmap[bo++]);
        }
        if (bits & 0x80) {
          if (size_x == 1 && size_y == 1) {
            int16_t pointer = (xx) + (yy)*w_;
            buffer[ pointer ] = color;
            // writePixel(x + xo + xx, y + yo + yy, color);
          } else {
            for(uint8_t _y=0; _y < size_y; _y++) {
              for(uint8_t _x=0; _x < size_x; _x++) {
                int16_t pointer = _x + xx*size_x + (yy*size_y + _y )*w_;
                buffer[ pointer ] = color;
              }
            }
            // writeFillRect(x + (xo16 + xx) * size_x, y + (yo16 + yy) * size_y,
            //               size_x, size_y, color);
          }
        }

        bits <<= 1;

      }
    }
    free(buffer);
    setAddrWindow(x+xo, y+yo, w_, h_);
    writePixels(buffer, len);
    endWrite();

  } // End classic vs custom font
}
