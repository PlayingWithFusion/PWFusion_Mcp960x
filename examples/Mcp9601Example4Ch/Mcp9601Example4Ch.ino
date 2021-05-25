#include <Wire.h>
#include <PWFusion_Mcp960x.h>

#define NUM_THERMOCOUPLES   4
Mcp960x thermo[NUM_THERMOCOUPLES];
const uint8_t mcp9601Addr[] = {0, 1, 4, 7};

void setup() {
  // Initialize I2C and serial port
  Wire.begin();
  Wire.setClock(100000L);
  
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect. Needed for native USB port only

  // Initialize MCP9601
  for (int i=0; i<NUM_THERMOCOUPLES; i++)
  {
    thermo[i].begin(mcp9601Addr[i]);
    if (thermo[i].isConnected()) {
      Serial.print(F("Found MCP9601 sensor at address "));
      Serial.println(i);

      thermo[i].setThermocoupleType(TYPE_K);
      thermo[i].setResolution(RES_18BIT, RES_0p0625);
    }
    else {
      Serial.print(F("ERROR: Unable to connect to MCP9601 sensor at address "));
      Serial.println(i);
    }
  }
}

void loop() {  
  for (int i=0; i<NUM_THERMOCOUPLES; i++)
  {
    Serial.print(F("Thermocouple "));
    Serial.print(i);
    Serial.print(F(" Temperature: "));
    
    switch (thermo[i].getStatus()) {
      case OPEN_CIRCUIT:
        Serial.println(F("Open Circuit"));
        break;
  
      case SHORT_CIRCUIT:
        Serial.println(F("Short Circuit"));
        break;
  
      case READY:
        Serial.println(thermo[i].getThermocoupleTemp());
        break;
  
      default:
        Serial.println(F("Pending"));
        break;
    }
  
    Serial.print(F("Ambient Temperature: "));
    Serial.println(thermo[i].getColdJunctionTemp());
  }
  Serial.println();

  delay(500);
}
