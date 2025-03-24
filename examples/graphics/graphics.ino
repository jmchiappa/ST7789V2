
#include <Arduino.h>
#include <SPI.h>
#include <ST7789V2.h>
#include <Adafruit_GFX.h>
#include <GFXFonts.h>
#include <Fonts/PoppinsMedium12pt7b.h>
// #include "img.h"

#define CS_PIN  D4
#define RESET_PIN D3
#define DC_PIN D2

SPISettings onlyEmitter(40000000,MSBFIRST,SPI_MODE0,true);

ST7789V2 myTFT( &SPI, CS_PIN, DC_PIN, RESET_PIN);

void setup() {
    SPI.beginTransaction(onlyEmitter);
    Serial.begin(115200);
    Serial.println("start setup...");
    myTFT.init(240,320);
    Serial.println("cleanup screen");
    myTFT.FillScreenFast(ST77XX_BLACK);
    delay(1000);
    Serial.println("Ready !");
    myTFT.FillRectangleFast(0,0,128,30,ST77XX_WHITE);
    myTFT.setFont(&PoppinsMedium12pt7b);
    myTFT.setTextColor(0xda28,ST77XX_WHITE);
    myTFT.setTextSize(1);
    myTFT.setCursor(0,20);
    myTFT.print("HELLO");

// swap pixel LSB/MSB
    // for( uint16_t i=0; i< Nom_HEIGHT * Nom_WIDTH ; i++) {
    //     uint16_t hi = (Nom[i] & 0xFF00) >> 8;
    //     uint16_t lo =  Nom[i] & 0xFF;
    //     Nom[i] = lo<<8 | hi;
    // }
}

void loop() {
  myTFT.FillScreenFast(ST77XX_BLUE);
  myTFT.FillScreenFast(ST77XX_GREEN);
  myTFT.FillScreenFast(ST77XX_RED);
    // Check border

    // myTFT.setFont(&PoppinsMedium7pt7b);
    // myTFT.setTextColor(0xda28,0x0);
    // myTFT.setTextSize(1);
    // myTFT.setCursor(10,10);
    // myTFT.print("Bonjour");


    // myTFT.drawFastHLine(0,0,myTFT.getWidth(),ST7735_RED);
    // myTFT.drawFastHLine(0,myTFT.getHeight()-1,myTFT.getWidth(),ST7735_RED);
    // myTFT.drawFastVLine(0,0,myTFT.getHeight(),ST7735_RED);
    // myTFT.drawFastVLine(myTFT.getWidth()-1,0,myTFT.getHeight(),ST7735_RED);
    // delay(1000);

    // myTFT.FillScreenFast(ST7735_BLACK);
    // delay(500);
    // myTFT.FillScreenFast(ST7735_BLACK);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_BLUE);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_RED);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_GREEN);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_CYAN);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_MAGENTA);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_YELLOW);
    // delay(500);

    // myTFT.FillScreenFast(ST7735_WHITE);
    // delay(500);

    // myTFT.drawRGBBitmap(0,0,Nom,Nom_WIDTH,Nom_HEIGHT);
    // delay(3000);
}