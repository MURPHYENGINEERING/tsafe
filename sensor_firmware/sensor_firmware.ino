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
bool showedConnectedMsg = false;


void setup() {
  pinMode(PIN_LED, OUTPUT);

  Serial.begin(115200);
  Serial.println("\n\n============ TSAFE ============");

  SerialBT.begin("TSAFE-SENSOR", true);
  SerialBT.setPin("0000");
}


static void try_connect()
{
  showedConnectedMsg = false;

  Serial.println("Connecting to display...");

  connected = SerialBT.connect(address);

  digitalWrite(PIN_LED, HIGH);
  delay(100);
  digitalWrite(PIN_LED, LOW);
  delay(100);
}


static void handle_connection()
{
  connected = SerialBT.connected();
  if (connected && !showedConnectedMsg) {
    Serial.println("Connected!");
    showedConnectedMsg = true;
  }

  digitalWrite(PIN_LED, HIGH);

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }
  if (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
}


void loop() {
  if (connected)
    handle_connection();
  else
    try_connect();
}
