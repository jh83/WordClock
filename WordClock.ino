#include <Ticker.h>
#include <Time.h>
#include <TimeLib.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <WiFiServer.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ws2812_i2s.h>

#include "settings.h"
#include "mainPage.h"
//#include "display.h"
#include "ntp.h"
#include "staticScreens.h"

#define SETUP_PIN 14
#define RADAR_PIN 12

#define MODE_SETUP 0
#define MODE_CLOCK 1
int clockMode;
bool isUpdating = false;
bool debugEnabled = true;
long lastMovement = 0;
int movement = 0;
String lastMovementDateTime, lastMovementDate, lastMovementTime;

ESP8266WebServer server (80);

String httpUpdateResponse;

time_t prevDisplay = 0;

void setupAP() {
  Serial.println("Entering Setup Mode.");
  Serial.print("Initiating AP: ");
  Serial.println(WIFI_AP_NAME);
  Serial.println("IP: 192.168.4.1");
  clockMode = MODE_SETUP;
  WiFi.mode(WIFI_AP);
  WiFi.softAP(WIFI_AP_NAME);
  //  displayAP();
}


void setupSTA()
{
  char ssid[32];
  char psk[64];
  memset(ssid, 0, 32);
  memset(psk, 0, 64);
  // displayBusy(1);

  clockMode = MODE_CLOCK;
  WiFi.mode(WIFI_STA);

  settings.ssid.toCharArray(ssid, 32);
  settings.psk.toCharArray(psk, 64);
  if (WiFi.status() != WL_CONNECTED) {
    if (settings.psk.length()) {
      WiFi.begin(ssid, psk);
    } else {
      WiFi.begin(ssid);
    }
  }

  Serial.print("Connecting to SSID: ");
  Serial.println(ssid);

  // Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);

    // Show the no WiFi screen
    showNoWifi();

    // Print some debug info
    Serial.print(WiFi.status());
    Serial.print(".");
  }

  // Once connected to WiFi
  Serial.print("Connected to WiFi. IP: ");
  Serial.println(WiFi.localIP());

  // Cleard the display
  clearDisplay();

  //  stopDisplayBusy();
  //  displayDash();
}

void setupWiFi() {
  // Wait up to 5s for GPIO0 to go low to enter AP/setup mode.
  //  displayBusy(0);
  while (millis() < 5000) {
    if (digitalRead(SETUP_PIN) == 0 || !settings.ssid.length()) {
      //      stopDisplayBusy();
      Serial.println("Setup Button pushed.");
      return setupAP();
    }
    delay(100);
  }
  //  stopDisplayBusy();
  setupSTA();
}


void handleRoot() {

  String s = MAIN_page;
  s.replace("@@SSID@@", settings.ssid);
  s.replace("@@PSK@@", settings.psk);
  s.replace("@@TZ@@", String(settings.timezone));
  s.replace("@@YEAR@@", String(year()));
  s.replace("@@MONTH@@", String(month()));
  s.replace("@@DAY@@", String(day()));
  s.replace("@@HOUR@@", String(hour()));
  s.replace("@@MIN@@", String(minute()));
  s.replace("@@SECOND@@", String(second()));
  s.replace("@@LASTMOVEMENT@@", lastMovementDateTime);
  s.replace("@@NTPSRV@@", settings.timeserver);
  s.replace("@@NTPINT@@", String(settings.interval));
  s.replace("@@SYNCSTATUS@@", timeStatus() == timeSet ? "OK" : "Overdue");
  s.replace("@@CLOCKNAME@@", settings.name);
  s.replace("@@ITIS@@", settings.showItIs == 1 ? "checked" : "");
  s.replace("@@DIMLVL@@", String(settings.dimLevel));
  s.replace("@@LEDTIME@@", String(settings.ledTime));
  s.replace("@@UPDATERESPONSE@@", httpUpdateResponse);
  httpUpdateResponse = "";
  server.send(200, "text/html", s);
}

void handleForm() {
  String update_wifi = server.arg("update_wifi");
  String t_ssid = server.arg("ssid");
  String t_psk = server.arg("psk");
  String t_timeserver = server.arg("ntpsrv");
  t_timeserver.toCharArray(settings.timeserver, EEPROM_TIMESERVER_LENGTH, 0);
  if (update_wifi == "1") {
    settings.ssid = t_ssid;
    settings.psk = t_psk;
  }
  String tz = server.arg("timezone");

  if (tz.length()) {
    settings.timezone = tz.toInt();
  }

  time_t newTime = getNtpTime();
  if (newTime) {
    setTime(newTime);
  }
  String syncInt = server.arg("ntpint");
  settings.interval = syncInt.toInt();

  settings.name = server.arg("clockname");
  settings.name.replace("+", " ");

  settings.showItIs = server.arg("showitis").toInt();

  settings.dimLevel = server.arg("dimlvl").toInt();

  settings.ledTime = server.arg("ledTime").toInt();

  httpUpdateResponse = "The configuration was updated.";

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Moved");

  settings.Save();

  updateDisplay();

  if (update_wifi == "1") {
    delay(500);
    setupWiFi();
  }
}

void setup() {

  if (debugEnabled == true)
  {
    Serial.begin(115200);
    Serial.println();
    Serial.println("Booting");
  }

  //Load settings from EEPROM
  Serial.println("Loading settings from EEPROM");
  settings.Load();

  // Hostname defaults to esp8266-[ChipID]
  String deviceName = settings.name;
  char deviceNameToCharArray[sizeof(deviceName)];
  deviceName.toCharArray(deviceNameToCharArray, sizeof(deviceNameToCharArray));
  ArduinoOTA.setHostname(deviceNameToCharArray);
  //  ArduinoOTA.setHostname(deviceName);

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
    isUpdating = true;
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("End");
    isUpdating = false;
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\n", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
    isUpdating = false;
  });
  ArduinoOTA.begin();

  // Initiate display and make sure that all LEDs are off
  leds.init(NUM_LEDS);
  clearDisplay();

  // Declare Pin Modes
  pinMode(SETUP_PIN, INPUT_PULLUP);
  pinMode(RADAR_PIN, INPUT);
  digitalWrite(SETUP_PIN, HIGH);

  // Attach the interrupt for the movement sensor
  attachInterrupt(RADAR_PIN, movementDetected, CHANGE);

  setupWiFi();
  setupTime();
  server.on("/", handleRoot);
  server.on("/form", handleForm);
  server.begin();
}


void loop() {
  ArduinoOTA.handle();
  server.handleClient();

  if (isUpdating == false)
  {
    //Serial.println("1");
    //if (displayIP()) return;
    if (clockMode == MODE_CLOCK) {
      if (timeStatus() != timeNotSet) {
        if (minute() != prevDisplay) { //update the display only if time has changed
          prevDisplay = minute();
          updateDisplay();
          //          displayClock();
        }
      }
    }

    //turn on/off display depending on movement
    // If movement is detected, then turn the display on by setting the dimLevel to configured level
    if (movement == 1)
    {
      movement = 2;
      settings.Load();
      updateDisplay();
    }

    // If no movement has been detected during the configured time, the turn the display off by lowering the dimLevel
    else if (movement == 2 && (lastMovement + (settings.ledTime * 60000)) < millis())
    {
      // Set the dim level to zero and then update the display to turn it off
      settings.dimLevel = 0;
      updateDisplay();

      // Set the movement flag back to 0
      movement = 0;
    }
  }
}

void movementDetected()
{
  movement = 1;
  lastMovement = millis();
  lastMovementDate = String(year()) + "-" + String(month()) + "-" + String(day());
  lastMovementTime = String(hour()) + ":" + String(minute()) + ":" + String(second());
  lastMovementDateTime = lastMovementDate + " " + lastMovementTime;
}

