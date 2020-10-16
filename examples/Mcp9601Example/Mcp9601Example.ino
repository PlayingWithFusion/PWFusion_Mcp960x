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
     thermo1.setResolution(RES_18BIT, RES_0p0625);
  }
  else {
    Serial.println(F("ERROR: Unable to connect to MCP9601 sensor"));
  }
}

void loop() {  
  Serial.print(F("Thermocouple Temperature: "));
  
  switch (thermo1.getStatus()) {
    case OPEN_CIRCUIT:
      Serial.println(F("Open Circuit"));
      break;

    case SHORT_CIRCUIT:
      Serial.println(F("Short Circuit"));
      break;

    case READY:
      Serial.println(thermo1.getThermocoupleTemp());
      break;

    default:
      Serial.println(F("Pending"));
      break;
  }

  Serial.print(F("Ambient Temperature: "));
  Serial.println(thermo1.getColdJunctionTemp());
  Serial.println();

  delay(500);
}
