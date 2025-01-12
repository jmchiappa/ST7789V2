#include "ST7735V2.h"
#include "Adafruit_ST7735.h"

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

ST7735V2::ST7735V2(SPIClass *spiClass, int8_t cs, int8_t dc, int8_t rst)
  : Adafruit_ST7735(spiClass,  cs,  dc,  rst ) {
      cs_ = digitalPinToPinName(cs);
      dc_ = digitalPinToPinName(dc);
      reset_ = digitalPinToPinName(rst);
  }

// void ST7735V2::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) {
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

void ST7735V2::FillRectangleFast(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
    // clipping
    // if((x >= Adafruit_ST7735::_width) || (y >= Adafruit_ST7735::_height)) return;
    // if((x + w - 1) >= Adafruit_ST7735::_width) w = Adafruit_ST7735::_width - x;
    // if((y + h - 1) >= Adafruit_ST7735::_height) h = Adafruit_ST7735::_height - y;

    startWrite();
    setAddrWindow(x, y, x+w, y+h);

    // Prepare whole line in a single buffer
    uint8_t pixel[] = { color >> 8, color & 0xFF };
    uint8_t *buffer = (uint8_t *)malloc(h * w * sizeof(pixel));
    for(x = 0; x < w * h; ++x)
    	memcpy(buffer + x * sizeof(pixel), pixel, sizeof(pixel));

    // digitalWriteFast(dc_,HIGH);
    hwspi._spi->transfer( buffer, h * w * sizeof(pixel));

    free(buffer);
    endWrite();
}

void ST7735V2::FillScreenFast(uint16_t color) {
    FillRectangleFast(0, 0, Adafruit_ST7735::_width, Adafruit_ST7735::_height, color);
}

void ST7735V2::drawRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[], int16_t w, int16_t h) {
  startWrite();
  setAddrWindow(x,y,w,h);
  hwspi._spi->transfer( (void *)bitmap, w * h * sizeof(uint16_t));
  endWrite();
}

// void ST7735V2::ST7735_WriteCommand(uint8_t cmd) {
//   digitalWriteFast(dc_,LOW);
//   hwspi._spi->transfer(&cmd, sizeof(cmd));

// }

// void ST7735V2::ST7735_WriteData(uint8_t* buff, size_t buff_size) {
//   digitalWriteFast(dc_,HIGH);
//   hwspi._spi->transfer(buff, buff_size);
// }

// void ST7735V2::ST7735_ExecuteCommandList(const uint8_t *addr) {
//     uint8_t numCommands, numArgs;
//     uint16_t ms;

//     numCommands = *addr++;
//     while(numCommands--) {
//         uint8_t cmd = *addr++;
//         ST7735_WriteCommand(cmd);

//         numArgs = *addr++;
//         // If high bit set, delay follows args
//         ms = numArgs & DELAY;
//         numArgs &= ~DELAY;
//         if(numArgs) {
//             ST7735_WriteData((uint8_t*)addr, numArgs);
//             addr += numArgs;
//         }

//         if(ms) {
//             ms = *addr++;
//             if(ms == 255) ms = 500;
//             HAL_Delay(ms);
//         }
//     }
// }