// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char SSID[]     = SECRET_SSID;    // Network SSID (name)
const char PASS[]     = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)

void onRegionChange();

String region;
int count_eu;
int count_swiss;
int count_unknown;

void initProperties(){

  ArduinoCloud.addProperty(region, READWRITE, ON_CHANGE, onRegionChange);
  ArduinoCloud.addProperty(count_eu, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(count_swiss, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(count_unknown, READ, ON_CHANGE, NULL);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
