#include <Arduino.h>
#include <TFT_eSPI.h>                 // Include the graphics library (this includes the sprite functions)

TFT_eSPI    tft = TFT_eSPI();         // Create object "tft"


void setup() {
  // put your setup code here, to run once:
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLUE);
}

void loop() {
  // put your main code here, to run repeatedly:
}