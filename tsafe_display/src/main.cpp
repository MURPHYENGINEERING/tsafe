#include <Arduino.h>
#include <BluetoothSerial.h>
#include <TFT_eSPI.h> // Include the graphics library (this includes the sprite functions)

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
  #error Bluetooth is not enabled! Please run `make menuconfig` to enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
  #error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

TFT_eSPI tft = TFT_eSPI(); // Create object "tft"

BluetoothSerial SerialBT;
#define BT_DISCOVER_TIME 10000
esp_spp_sec_t sec_mask
    = ESP_SPP_SEC_NONE; // or ESP_SPP_SEC_ENCRYPT|ESP_SPP_SEC_AUTHENTICATE to
                        // request pincode confirmation
esp_spp_role_t role = ESP_SPP_ROLE_SLAVE; // or ESP_SPP_ROLE_MASTER

void
setup()
{
  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_RED);

  Serial.begin(115200);

  if (!SerialBT.begin("TSAFE", false)) {
    Serial.println("! Couldn't start BluetoothSerial");
    abort();
  }
}

void
loop()
{
  if (SerialBT.connected()) {
    tft.fillScreen(TFT_BLUE);
  } else {
    tft.fillScreen(TFT_RED);
  }

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }

  delay(20);
}