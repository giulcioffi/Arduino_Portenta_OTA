/*
 This example demonstrates how to use to update
 the firmware of the Arduino Portenta H7 using
 a firmware image stored on the QSPI.
 */
#include "ArduinoOTAPortenta.h"

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("*****OTA from QSPI Flash*****");
  ArduinoOTAPortenta_QSPI ota(QSPI_FLASH_FATFS_MBR, 2);
  ota.setUpdateLen(131728);

  pinMode(LEDB, OUTPUT);
  digitalWrite(LEDB, LOW);
  delay(5000);
  digitalWrite(LEDB, HIGH);

  ota.update();
}

void loop()
{

}
