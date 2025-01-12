#ifndef __ST7735V2__
#define __ST7735V2__

#include "Adafruit_ST7735.h"
#include "SPI.h"

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
  protected:
    
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