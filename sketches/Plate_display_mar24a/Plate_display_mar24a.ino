#include "arduino_secrets.h"

// Change CLOUD_ENABLE to 0 to make the sketch work offline, w/o connection to the cloud
#define CLOUD_ENABLE 1

#if CLOUD_ENABLE
  #include "thingProperties.h"
#else
  String region;
  int count_eu;
  int count_swiss;
  int count_unknown;
#endif

#include <Arduino_GigaDisplay.h>
#include "Arduino_GigaDisplay_GFX.h"

GigaDisplayRGB rgb;
GigaDisplay_GFX display;

#define CENTER_X (800 / 2)
#define CENTER_Y (400 / 2)
#define SWISS_FLAG_THICKNESS 60
#define SWISS_FLAG_LENGTH 180
#define UE_FLAG_RADIUS 120

#define BLACK 0x0000
#define WHITE 0xffff
#define YELLOW 0xFFE0
#define BLUE 0x001f
#define RED 0xf800
#define ARDUINO_COLOR 0x64f3

// for how long we want to display a flag on display
#define SHOW_FLAG_TIME (2*1000)

// how many seconds to display the "connection lost" screen
#define DISCONNECTION_TIME (10*1000)

String REGION_EU = "EU";
String REGION_SWISS = "SWISS";
String REGION_UNKNOWN = "UNKNOWN";
String REGION_PREDICTION_FAILED = "FAIL";
String UNEXPECTED_MESSAGE = "Unexpected ";

long timeOfLastMeaningfulResult = 0;
bool waitingScreenDrawn = false;
bool disconnectionScreenDrawn = false;

bool lastValueDiscarded = false; // probably not useful anymore

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial1.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);
  rgb.begin();
  rgb.on(0, 0, 255);  // blue means no connection
  display.begin();
  display.setRotation(1);
  drawWelcomeScreen("Welcome to Arduino");

  #if CLOUD_ENABLE
    initProperties();

    // Connect to Arduino IoT Cloud
    ArduinoCloud.begin(ArduinoIoTPreferredConnection);
    ArduinoCloud.addCallback(ArduinoIoTCloudEvent::CONNECT, onConnect);
    ArduinoCloud.addCallback(ArduinoIoTCloudEvent::SYNC, onSync);
    ArduinoCloud.addCallback(ArduinoIoTCloudEvent::DISCONNECT, onDisconnect);

    setDebugMessageLevel(4);
    ArduinoCloud.printDebugInfo();
  #endif

  drawWelcomeScreen("Connecting...");
}

void loop() {
  #if CLOUD_ENABLE
    ArduinoCloud.update();
  #endif

  long now = millis();

  if (Serial1.available()) {
    delay(2);
    disconnectionScreenDrawn = false;
    char inByte = Serial1.read();
    Serial.print(" -> [");
    Serial.print(inByte);  //Print data on Serial Monitor
    Serial.println("] <-");
    if (inByte == 'E') {
      region = REGION_EU;
      count_eu += 1;
      timeOfLastMeaningfulResult = now;
      drawEUFlag();
      waitingScreenDrawn = false;
      rgb.on(0, 255, 0);
    } else if (inByte == 'S') {
      region = REGION_SWISS;
      count_swiss += 1;
      timeOfLastMeaningfulResult = now;
      drawSwissFlag();
      waitingScreenDrawn = false;
      rgb.on(0, 255, 0);
    } else if (inByte == 'U') {
      region = REGION_UNKNOWN;
      count_unknown += 1;
      timeOfLastMeaningfulResult = now;
      drawUnknownResult();
      waitingScreenDrawn = false;
      rgb.on(255, 0, 0);
    } else if (inByte == 'F') {
      region = REGION_PREDICTION_FAILED;
      timeOfLastMeaningfulResult = now;
      drawWelcomeScreen("Prediction failed");
      waitingScreenDrawn = false;
      rgb.on(255, 0, 0);
    } else {
      region = REGION_PREDICTION_FAILED;
      timeOfLastMeaningfulResult = now;
      drawWelcomeScreen(UNEXPECTED_MESSAGE += inByte);
      waitingScreenDrawn = false;
      rgb.off();
    }
  }

  if (!disconnectionScreenDrawn && (now - timeOfLastMeaningfulResult >= DISCONNECTION_TIME)) {
      disconnectionScreenDrawn = true;
      region = REGION_PREDICTION_FAILED;
      drawWelcomeScreen("No input");
      rgb.on(255, 0, 0);
  }
  if (!waitingScreenDrawn && !disconnectionScreenDrawn && (now - timeOfLastMeaningfulResult >= SHOW_FLAG_TIME)) {
      waitingScreenDrawn = true;
      region = REGION_UNKNOWN;
      drawWelcomeScreen("Waiting...");
      rgb.on(255, 0, 0);
  }
}

void onSync() {
  Serial.print("Syncing [");
  Serial.print(region);
  Serial.println("]");
  
}

void onConnect() {
  /* add your custom code here */
  Serial.println("Board successfully connected to Arduino IoT Cloud");
  region = REGION_PREDICTION_FAILED;
  rgb.off();
  drawWelcomeScreen("Connected, waiting...");
}
void onDisconnect() {
  /* add your custom code here */
  Serial.println("Board disconnected from Arduino IoT Cloud");
  drawWelcomeScreen("Disconnected...");
  rgb.on(0, 0, 255);
}
/*
  Since Region is READ_WRITE variable, onRegionChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onRegionChange() {
  // if (lastValueDiscarded) {
    // Add your code here to act upon Region change

    // Serial.print("Region is now: ");
    // Serial.println(region);

  // }
}

void drawWelcomeScreen(String message) {
  display.fillScreen(WHITE);
  display.setCursor(40, 400);  //x,y
  display.setTextSize(6);      //adjust text size
  display.setTextColor(ARDUINO_COLOR);
  display.print(message);

  display.fillCircle(CENTER_X - 100, CENTER_Y, 110, ARDUINO_COLOR);
  display.fillCircle(CENTER_X - 100, CENTER_Y, 70, WHITE);

  display.fillCircle(CENTER_X + 100, CENTER_Y, 110, ARDUINO_COLOR);
  display.fillCircle(CENTER_X + 100, CENTER_Y, 70, WHITE);

  display.setCursor(CENTER_X - 140, CENTER_Y - 56);  //x,y
  display.setTextSize(16);                           //adjust text size
  display.print("-");

  display.setCursor(CENTER_X + 60, CENTER_Y - 56);  //x,y
  display.setTextSize(16);                          //adjust text size
  display.print("+");

  display.setTextColor(WHITE);
}

void drawEUFlag() {
  display.fillScreen(BLUE);
  display.setCursor(350, 400);
  display.setTextSize(10);
  display.print("EU");

  for (int i = 0; i < 12; i++) {
    drawStar(CENTER_X + (UE_FLAG_RADIUS * cos(i * PI / 6)), CENTER_Y + (UE_FLAG_RADIUS * sin(i * PI / 6)));
  }
}

void drawStar(uint x, uint y) {
  display.setCursor(x, y);  //x,y
  display.setTextColor(YELLOW);
  display.setTextSize(3);
  display.print("*");
  display.setTextColor(WHITE);
}

void drawSwissFlag() {
  display.fillScreen(RED);
  display.setCursor(350, 400);
  display.setTextSize(10);
  display.print("CH");

  display.fillRect(CENTER_X - SWISS_FLAG_THICKNESS / 2, CENTER_Y - SWISS_FLAG_LENGTH / 2, SWISS_FLAG_THICKNESS, SWISS_FLAG_LENGTH, WHITE);
  display.fillRect(CENTER_X - SWISS_FLAG_LENGTH / 2, CENTER_Y - SWISS_FLAG_THICKNESS / 2, SWISS_FLAG_LENGTH, SWISS_FLAG_THICKNESS, WHITE);
}

void drawUnknownResult() {
  display.fillScreen(WHITE);
  display.setCursor(200, 400);  //x,y
  display.setTextSize(10);      //adjust text size
  display.setTextColor(RED);
  display.print("UNKNOWN");  //print

  display.fillCircle(CENTER_X, CENTER_Y, 110, RED);
  display.fillRect(CENTER_X - SWISS_FLAG_LENGTH / 2, CENTER_Y - SWISS_FLAG_THICKNESS / 2, SWISS_FLAG_LENGTH, SWISS_FLAG_THICKNESS, WHITE);

  display.setTextColor(WHITE);
}


void clearDisplay() {
  display.begin();
}

