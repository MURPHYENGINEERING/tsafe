
#include <BluetoothSerial.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define PIN_LED 2

BluetoothSerial SerialBT;
uint8_t address[6]  = {0x4c, 0xeb, 0xd6, 0x43, 0xe2, 0x8a};

bool connected = false;


void setup() {
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("\n============ TSAFE SENSOR ============");

  // Bluetooth module
  if (!SerialBT.begin("TSAFE-SENSOR", true)) {
    Serial.println("! Couldn't start BluetoothSerial");
    abort();
  }
  SerialBT.setPin("0000");

  Serial.println("? Bluetooth ready");
  connected = false;
}


static void try_connect()
{
  Serial.println("? Connecting to display module...");

  digitalWrite(PIN_LED, LOW);
  if (SerialBT.connect(address)) {
    connected = true;
    digitalWrite(PIN_LED, HIGH);
    Serial.println("? Connected!");
    return;
  }

  digitalWrite(PIN_LED, HIGH);
  delay(100);
  digitalWrite(PIN_LED, LOW);
  delay(100);
}


static void handle_connection()
{
  if (!SerialBT.connected()) {
    connected = false;
    Serial.println("! Disconnected from display module");
    return;
  }

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
}


void loop() {
  if (connected)
    handle_connection();
  else
    try_connect();
}
