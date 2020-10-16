#include <Wire.h>
#include <PWFusion_Mcp960x.h>

Mcp960x thermo1;

void setup() {
  // Initialize I2C and serial port
  Wire.begin();
  Wire.setClock(100000);
  Serial.begin(9600);

  // Initialize MCP9601 with address 1
  thermo1.begin(1);

  if (thermo1.isConnected()) {
     Serial.println(F("Found MCP9601 sensor"));

     thermo1.setThermocoupleType(TYPE_K);

     // Specify 12-bit resolution for quick measurements.  Each 12-bit 
     // measurment takes about 5ms.  Since we are capturing 32 smaples in burst
     // mode, the total conversion time is at least 160ms
     thermo1.setResolution(RES_12BIT, RES_0p0625);

     // Enable MCP9601 low-pass filter and start burst measurment
     Serial.println(F("Starting Burst measurment."));
     thermo1.setFilterCoeffecients(1);
     thermo1.beginBurst(BURST_32);
  }
  else {
    Serial.println(F("ERROR: Unable to connect to MCP9601 sensor"));
  }
}

void loop() {
  if (thermo1.burstComplete()) {
     Serial.print(F("Thermocouple Temperature: "));
  
    switch (thermo1.getStatus()) {
      case OPEN_CIRCUIT:
        Serial.println(F("Open Circuit"));
        break;

      case SHORT_CIRCUIT:
        Serial.println(F("Short Circuit"));
        break;

      default:
        Serial.println(thermo1.getThermocoupleTemp());
        break;
    }

    Serial.print(F("Ambient Temperature:"));
    Serial.println(thermo1.getColdJunctionTemp());
    Serial.println();

    // Wait one second then begin the next burst measurment
    delay(1000);
    thermo1.beginBurst(BURST_32);
    
  }
}
