// ClientReadPowerFlow.ino

/***********************************************************************************
  Test HTTP Client on ESP32 as Wifi station in a local network.
  Decode JSON data sent by the Fronius API of a GEN24 inverter.

  References :
  - https://randomnerdtutorials.com/esp32-http-get-post-arduino/  
  - https://github.com/arduino-libraries/Arduino_JSON 
  - https://github.com/pfeerick/elapsedMillis
  
***********************************************************************************/

/***********************************************************************************
  Libraries and structures
***********************************************************************************/

// Wifi
#include <WiFi.h>

// HTTPClient
#include <HTTPClient.h>

// JSON
#include <Arduino_JSON.h>

// Timer
#include <elapsedMillis.h>

/***********************************************************************************
  Constants
***********************************************************************************/

// Local network access point
const char *SSID = "Livebox-486c";
const char *PWD = "FAAD971F372AA5CCE13D25969E";

// Wifi period (ms)
#define WIFI_PERIOD 1000    

/***********************************************************************************
  Global variables
***********************************************************************************/

// Parameters
double pv;   // PV production
double load; // Consommation
double akku; // Battery
double grid; // Grid


// Timer
elapsedMillis tmrWifi;

/***********************************************************************************
  Functions
***********************************************************************************/

void setup()
{
  // Connect to the Wifi access point 
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) delay(500);

  // Open serial port
  Serial.begin(115200);
  while (!Serial) {}
}

void loop()
{
  if (tmrWifi > WIFI_PERIOD)
  {
    // Reset timer
    tmrWifi = 0;
    
    // Reconnect Wifi if necessary
    if (WiFi.status() == WL_CONNECTED) 
    {
      // GET request
      HTTPClient http; 
      http.begin("http://192.168.1.13/solar_api/v1/GetPowerFlowRealtimeData.fcgi");
      if(http.GET() == HTTP_CODE_OK)
      {
        // Decode data
        String s = http.getString();
        pv = (double)(JSON.parse(s)["Body"]["Data"]["Site"]["P_PV"]);
        load = (double)(JSON.parse(s)["Body"]["Data"]["Site"]["P_Load"]);
        akku = (double)(JSON.parse(s)["Body"]["Data"]["Site"]["P_Akku"]);
        grid = (double)(JSON.parse(s)["Body"]["Data"]["Site"]["P_Grid"]);
        

        // Display data
        Serial.print("PV : "); Serial.print(pv); Serial.print("W  ");
        Serial.print("Load : "); Serial.print(load); Serial.print("W  ");
        Serial.print("Akku : "); Serial.print(akku); Serial.print("W  ");   
        Serial.print("Grid : "); Serial.print(grid); Serial.println("W");        
      }
      http.end();  
    }
    else WiFi.reconnect();
  }
}
