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
    #ifdef HALVOE_DVI_DEBUG
      Serial.println("beginDVI() failed!");
    #endif // HALVOE_DVI_DEBUG
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); delay(250); }
  }

  if (not halvoeDVI::AtPico::beginSPI())
  {
    #ifdef HALVOE_DVI_DEBUG
      Serial.println("beginSPI() failed!");
    #endif // HALVOE_DVI_DEBUG
    while (true) { digitalWrite(LED_BUILTIN, HIGH); delay(250); digitalWrite(LED_BUILTIN, LOW); delay(250); }
  }

  #ifdef HALVOE_DVI_DEBUG
    Serial.print("CPU frequency: ");
    Serial.print(rp2040.f_cpu());
    Serial.println(" hz");
    Serial.print("Free heap: ");
    Serial.print(rp2040.getFreeHeap());
    Serial.println(" bytes");
    Serial.print("Free stack: ");
    Serial.print(rp2040.getFreeStack());
    Serial.println(" bytes");
  #endif // HALVOE_DVI_DEBUG

  #ifdef HALVOE_DVI_DEBUG
    Serial.println("PicoDVI leaves setup().");
  #endif // HALVOE_DVI_DEBUG

  halvoeDVI::AtPico::setupIsDVIReadyPin();
  halvoeDVI::AtPico::writeIsDVIReady(true);
}

void loop()
{
  // nothing do to here - interrupt handles buffer swapping
}
