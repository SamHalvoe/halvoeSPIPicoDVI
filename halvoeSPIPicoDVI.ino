#include "halvoeAtPicoDVI.hpp"

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef HALVOE_DVI_DEBUG
    Serial.begin(115200);
    while (not Serial) { delay(500); }
    Serial.println("PicoDVI Serial to USB is ready.");
    Serial.print("Build Version: ");
    Serial.println(halvoeDVI::buildVersion);
    Serial.print("Build Timestamp: ");
    Serial.println(halvoeDVI::buildTimestamp);
  #endif // HALVOE_DVI_DEBUG

  if (not halvoeDVI::AtPico::beginDVI())
  {
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); delay(250); }
  }

  if (not halvoeDVI::AtPico::beginSPI())
  {
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); delay(250); }
  }

  #ifdef HALVOE_DVI_DEBUG
    Serial.println("PicoDVI leaves setup().");
  #endif // HALVOE_DVI_DEBUG
}

void loop()
{
  // receiving and transfering via SPI is handled by callbacks - so nothing to do here
}
