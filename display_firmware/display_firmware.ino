#include <Arduino.h>
#include <BluetoothSerial.h>
#include <TFT_eSPI.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif


#define DWIDTH 135
#define DHEIGHT 240

bool connected;

// Display driver
TFT_eSPI tft       = TFT_eSPI();
// Image data, with backbuffer
TFT_eSprite sprite = TFT_eSprite(&tft);
uint8_t* spriteImage;

size_t spriteCursor;

// Bluetooth driver
BluetoothSerial btSerial;
#define READBUF_SIZE 33000
char readBuf[READBUF_SIZE];


void
setup()
{
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_RED);

  Serial.begin(9600);
  Serial.println();
  Serial.println("========== TSAFE ==========");

  if (!btSerial.begin("TSAFE", false)) {
    Serial.println("! Couldn't start BluetoothSerial");
    abort();
  }
  Serial.println("? Bluetooth ready");
  connected = false;

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


void
loop()
{
  if (!btSerial.connected()) {
    tft.fillScreen(TFT_RED);
    delay(100);
    if (connected) {
      Serial.println("? Bluetooth disconnected");
      connected = false;
    }
    return;

  } else if (!connected) {
    Serial.println("? Bluetooth connected!");
    tft.fillScreen(TFT_BLACK);
    connected = true;
  }

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