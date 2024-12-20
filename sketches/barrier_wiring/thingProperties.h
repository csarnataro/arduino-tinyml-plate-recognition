// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)

void onRegionChange();

String region;

void initProperties(){

  ArduinoCloud.addProperty(region, READWRITE, ON_CHANGE, onRegionChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
