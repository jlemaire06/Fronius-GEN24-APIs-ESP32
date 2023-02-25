// ClientReadHoldingRegisters.ino

/***********************************************************************************
  Modbus TCP client (master) to read asynchronously contiguous holding registers in a
  Fronius GEN24 inverter (slave), using an ESP32 as Wifi station in a local network.

  References :
  - https://github.com/eModbus/eModbus
  
***********************************************************************************/

/***********************************************************************************
  Libraries and structures
***********************************************************************************/

// Wifi
#include <WiFi.h>

// Modbus Client TCP
#include <ModbusClientTCP.h>

/***********************************************************************************
  Constants
***********************************************************************************/

// Local network access point
const char *SSID = "Livebox-486c";
const char *PWD = "FAAD971F372AA5CCE13D25969E";

/***********************************************************************************
  Global variables
***********************************************************************************/

// Wifi client
WiFiClient theClient; 

// Create a ModbusTCP client instance
ModbusClientTCP MB(theClient);

/***********************************************************************************
  Functions
***********************************************************************************/

// Define an onData handler function to receive the regular responses
void handleData(ModbusMessage response, uint32_t token) 
{
  Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
  for (auto& byte : response) Serial.printf("%02X ", byte);
  Serial.println("");
}

// Define an onError handler function to receive error responses
void handleError(Error error, uint32_t token) 
{
  ModbusError me(error);
  Serial.printf("Error response: %02X - %s\n", (int)me, (const char *)me);
}

void setup()
{
    // Open serial port
  Serial.begin(115200);
  while (!Serial) {}
  
  // Connect to the Wifi access point 
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Set up ModbusTCP client
  MB.onDataHandler(&handleData); 
  MB.onErrorHandler(&handleError); 
  MB.setTimeout(2000, 200); // Message timeout and interval between requests to the same host
  MB.begin();

  // Issue a request
  MB.setTarget(IPAddress(192, 168, 1,13), 502); // Modbus Server address and port
  uint8_t serverID = 1;
  uint8_t functionCode = READ_HOLD_REGISTER; // 0x03  
  uint16_t startAddress = 40355; // => OutWRte (#40356) = percent of max decharge rate
  uint16_t nbRegisters = 1;  // Nb of registers
  Error err = MB.addRequest((uint32_t)millis(), serverID, functionCode, startAddress, nbRegisters);
  if (err!=SUCCESS) 
  {
    ModbusError e(err);
    Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
  }
}

void loop() {}
