#include "halvoeInfo.hpp"
#include "halvoeAtPicoDVI.hpp"

halvoeDVI::AtPico::SPILink spiLink;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef HALVOE_DVI_DEBUG
    Serial.begin(115200);
    while (not Serial) { delay(1000); }
    Serial.println("PicoDVI Serial to USB is ready.");
    Serial.print("Build Version: ");
    Serial.println(halvoeDVI::buildVersion);
    Serial.print("Build Timestamp: ");
    Serial.println(halvoeDVI::buildTimestamp);
  #endif // HALVOE_DVI_DEBUG

  if (not spiLink.begin())
  {
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); delay(250); }
  }
}

void loop()
{
  // receiving and transfering via SPI is handled by callbacks - so nothing to do here
}
