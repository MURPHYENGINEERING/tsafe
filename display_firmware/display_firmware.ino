#include <Arduino.h>
#include <BluetoothSerial.h>
#include <TFT_eSPI.h>
// You must go to the TTF_eSPI library folder and edit the file
// User_Setup_Select.h:
// Comment #include <User_Setup.h>
// Uncomment #include <User_Setups/Setup25_TTGO_T_Display.h>


#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define DWIDTH 135
#define DHEIGHT 240

// Display driver
TFT_eSPI tft       = TFT_eSPI();
// Image data, with backbuffer
TFT_eSprite sprite = TFT_eSprite(&tft);
uint8_t* spriteImage;

size_t spriteCursor;

// Bluetooth driver
BluetoothSerial btSerial;
bool connected;
#define READBUF_SIZE 33000
char readBuf[READBUF_SIZE];


void
setup()
{
  Serial.begin(115200);
  Serial.println("\n============ TSAFE DISPLAY ============");

  // Bluetooth module
  if (!btSerial.begin("TSAFE-DISPLAY", false)) {
    Serial.println("! Couldn't start BluetoothSerial");
    abort();
  }
  Serial.println("? Bluetooth ready");
  connected = false;

  // Display module
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_RED);

  spriteImage = (uint8_t*) sprite.createSprite(DWIDTH, DHEIGHT);
  if (!spriteImage) {
    Serial.println("! Couldn't create sprite");
    abort();
  }
  sprite.setColorDepth(8);
  spriteCursor = 0;

  // Hold TFT chip select down
  tft.startWrite();

  Serial.println("? Display ready");
}


void try_receive_image_data()
{
  if (!btSerial.available())
    return;

  size_t bytesRead = btSerial.readBytes(readBuf, READBUF_SIZE);
  for (size_t i = 0; i < bytesRead; ++i) {
    uint8_t byte = readBuf[i];

    if (byte & 0b1000000) {
      // Frame preamble
      spriteCursor = 0;
      sprite.pushSprite(0, 0);
    } else {
      // Frame data
      if (spriteCursor < DWIDTH * DHEIGHT)
        // TODO: sample color gradient instead
        spriteImage[spriteCursor++] = byte;
    }
  }
}


void
loop()
{
  if (!btSerial.connected()) {
    tft.fillScreen(TFT_RED);
    if (connected)
      Serial.println("! Disconnected from sensor module");
    connected = false;
    Serial.println("? Waiting for sensor module...");
    delay(1000);
    return;

  } else if (!connected) {
    Serial.println("? Connected to sensor module");
    tft.fillScreen(TFT_BLUE);
    connected = true;
  }

  try_receive_image_data();
}