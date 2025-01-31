
#include <Arduino.h>
#include <SPI.h>
#include <ST7735V2.h>
#include <Adafruit_GFX.h>
#include <GFXFonts.h>
#include <Fonts/PoppinsMedium12pt7b.h>
#include "img.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#define CAPTEUR_THERMO D45

#define CS_PIN  D2
#define RESET_PIN D3
#define DC_PIN D4

SPISettings onlyEmitter(40000000,MSBFIRST,SPI_MODE0,true);

ST7735V2 myTFT( &SPI, CS_PIN, DC_PIN, RESET_PIN);

OneWire  ds_CAPTEUR_THERMO(CAPTEUR_THERMO);  // on pin CAPTEUR_THERMO (a 4.7K resistor is necessary)

float LastTemperature = -273;

DallasTemperature sensor_CAPTEUR_THERMO(&ds_CAPTEUR_THERMO);

void setup() {
    SPI.beginTransaction(onlyEmitter);
    Serial.begin(115200);

    pinMode(CAPTEUR_THERMO, INPUT);
    sensor_CAPTEUR_THERMO.begin();
    sensor_CAPTEUR_THERMO.setResolution(9);

    myTFT.initR(INITR_BLACKTAB);
    myTFT.FillScreenFast(ST7735_WHITE);
    Serial.println("Ready !");

// swap pixel LSB/MSB
    for( uint16_t i=0; i< Nom_HEIGHT * Nom_WIDTH ; i++) {
        uint16_t hi = (Nom[i] & 0xFF00) >> 8;
        uint16_t lo =  Nom[i] & 0xFF;
        Nom[i] = lo<<8 | hi;
    }
}

void loop() {
    // Check border

    // myTFT.setFont(&PoppinsMedium7pt7b);
    // myTFT.setTextColor(0xda28,0x0);
    // myTFT.setTextSize(1);
    // myTFT.setCursor(10,10);
    // myTFT.print("Bonjour");

    sensor_CAPTEUR_THERMO.requestTemperatures();
    if (sensor_CAPTEUR_THERMO.isConversionComplete()) {
        float temperature = sensor_CAPTEUR_THERMO.getTempCByIndex(0);
        if(temperature != LastTemperature) {
            myTFT.FillRectangleFast(0,0,128,21,ST7735_WHITE);
            myTFT.setFont(&PoppinsMedium12pt7b);
            myTFT.setTextColor(0xda28,ST7735_WHITE);
            myTFT.setTextSize(1);
            myTFT.setCursor(0,20);
            myTFT.print("T : " + String(temperature,1)  + " C");
            LastTemperature = temperature;
        }
    }

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