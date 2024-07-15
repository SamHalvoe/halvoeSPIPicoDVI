#include "halvoeAtPicoDVI.hpp"

halvoeDVI::AtPico::SPILink spiLink(SPISlave);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  #ifdef HALVOE_DVI_DEBUG
    Serial.begin(115200);
    while (not Serial) { delay(1000); }
    Serial.println("gpu serial to usb ready");
  #endif // HALVOE_DVI_DEBUG

  if (not spiLink.begin())
  {
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); }
  }
}

void loop()
{
  // receiving and transfering via SPI is handled by callbacks - so nothing to do here
}
