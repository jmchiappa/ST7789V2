## Goal of the library

It intends to overrides some SPI behavior of Adafruit ST7735 ilbrary.

## API

- SPI_CS HIGH / LOW
- SPI_DC HIGH / LOW

startWrite()

endWrite()

Override these functions :
- FillRectanglesFast
- FillScreenFast
- drawRGBBitmap

It consistss in sending the whole in one shot instead of byte by byte tothe SPi driver.
If the SPI DMA driver is isued, then it will drmaatically faster than the legacy Arduino's SPI driver.