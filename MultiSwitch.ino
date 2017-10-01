#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>

#include "WemoSwitch.h"
#include "WemoManager.h"
#include "CallbackFunction.h"

// prototypes
boolean connectWifi();

//------- Network params ------
const char* host = "esp8266-webupdate";
const char *ssid = "WLAN1_Guest1";       // your network SSID (name)
const char *password = "Locoto100";  // your network key

WemoManager wemoManager;
WemoSwitch *office = NULL;  // office lights
WemoSwitch *kitchen = NULL; // kitchen lights
WemoSwitch *led = NULL;     // coffee light
WemoSwitch *red = NULL;     // red light
WemoSwitch *light = NULL;   // pin4 test

const int relayPin1 = 16;
const int relayPin2 = 14;
const int ledPin = 2; //BUILTIN_LED;
const int redLED = 4;
const int lightPin = 4;

bool inOTA = false;

void OTAsetup() {
  //ArduinoOTA.setHostname("ESP8266");
  ArduinoOTA.setPassword((const char *)"esp8266");

  ArduinoOTA.onStart([]() {
    inOTA = true;
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    inOTA = false;
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA ready");
}

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  /*
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  */
  while(WiFi.waitForConnectResult() != WL_CONNECTED){
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  OTAsetup();
  
  wemoManager.begin();
  // Format: Alexa invocation name, local port no, on callback, off callback
  office = new WemoSwitch("office lights", 80, officeLightsOn, officeLightsOff);
  kitchen = new WemoSwitch("kitchen lights", 81, kitchenLightsOn, kitchenLightsOff);
  led = new WemoSwitch("coffee light", 82, ledOn, ledOff);
  red = new WemoSwitch("red light", 83, redOn, redOff);
  // light = new WemoSwitch("test lights", 84, lightOn, lightOff);

  
  wemoManager.addDevice(*office);
  wemoManager.addDevice(*kitchen);
  wemoManager.addDevice(*led);
  wemoManager.addDevice(*red);
  // wemoManager.addDevice(*light);
  
  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(lightPin, OUTPUT);
  delay(10);
  digitalWrite(ledPin, HIGH); // Wemos BUILTIN_LED is active Low, so high is off
}

unsigned long previousTime = millis();
const unsigned long interval = 200;

bool ota_flag = true;
uint16_t time_elapsed = 0;
int count = 0;
int numReadyBlinks = 10;

void loop()
{
  ArduinoOTA.handle();
  if (!inOTA) {
    wemoManager.serverLoop();
  }
}

void officeLightsOn() {
    Serial.print("Switch 1 turn on ...");
    digitalWrite(relayPin1, HIGH);
}

void officeLightsOff() {
    Serial.print("Switch 1 turn off ...");
    digitalWrite(relayPin1, LOW);
}

void kitchenLightsOn() {
    Serial.print("Switch 2 turn on ...");
    digitalWrite(relayPin2, HIGH);
}

void kitchenLightsOff() {
  Serial.print("Switch 2 turn off ...");
  digitalWrite(relayPin2, LOW);
}

void ledOn() {
    Serial.print("LED turn on ...");
    digitalWrite(ledPin, LOW);
}

void ledOff() {
  Serial.print("LED turn off ...");
  digitalWrite(ledPin, HIGH);
}

void redOn() {
    Serial.print("LED turn on ...");
    digitalWrite(redLED, HIGH);
}

void redOff() {
  Serial.print("LED turn off ...");
  digitalWrite(redLED, LOW);
}

void lightOn() {
    Serial.print("Switch 1 turn on ...");
    digitalWrite(lightPin, HIGH);
}

void lightOff() {
    Serial.print("Switch 1 turn off ...");
    digitalWrite(lightPin, LOW);
}
