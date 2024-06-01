#include <M5Atom.h>
#include <TinyGPS++.h>
#include <SD.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// GPS and SD card
TinyGPSPlus gps;
File logFile;
#define GPS_SERIAL Serial1
#define SD_CS_PIN 4

// NTP client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// Bluetooth
BluetoothSerial SerialBT;

// States and data
bool isLogging = false;
bool displayOn = true;
float prevLat = 0.0, prevLng = 0.0;
int filesRecordedThisWeek = 0;
String currentFilename;

void setup() {
  M5.begin();
  GPS_SERIAL.begin(9600);
  SD.begin(SD_CS_PIN);
  SerialBT.begin("GPSLogger");
  connectWiFi();
  timeClient.begin();
  M5.dis.display.setBrightness(30);
}

void loop() {
  M5.update();
  while (GPS_SERIAL.available() > 0) {
    gps.encode(GPS_SERIAL.read());
  }

  if (gps.location.isUpdated()) {
    float distance = TinyGPSPlus::distanceBetween(prevLat, prevLng, gps.location.lat(), gps.location.lng());
    if (distance > 2) {
      logData();
      prevLat = gps.location.lat();
      prevLng = gps.location.lng();
    }
  }

  if (M5.Btn.wasPressed()) {
    isLogging = !isLogging;
  }

  if (M5.BtnA.wasPressed()) {
    displayOn = !displayOn;
  }

  updateDisplay();
}

void logData() {
  if (!isLogging) return;

  String stateCity = "StateCity";  // Implement logic to determine nearest capital/state city
  String date = timeClient.getFormattedDate().substring(0, 10);
  currentFilename = "/" + date + "-" + stateCity + ".GPX";
  
  if (!SD.exists(currentFilename)) {
    logFile = SD.open(currentFilename, FILE_WRITE);
    logFile.println("<gpx version=\"1.1\" creator=\"M5Stack GPS Logger\">");
    filesRecordedThisWeek++;
  } else {
    logFile = SD.open(currentFilename, FILE_APPEND);
  }

  logFile.print("<trkpt lat=\"");
  logFile.print(gps.location.lat(), 6);
  logFile.print("\" lon=\"");
  logFile.print(gps.location.lng(), 6);
  logFile.print("\"><ele>");
  logFile.print(gps.altitude.meters());
  logFile.print("</ele><time>");
  logFile.print(timeClient.getFormattedTime());
  logFile.println("</time></trkpt>");

  logFile.close();
}

void updateDisplay() {
  if (!displayOn) {
    M5.dis.clear();
    return;
  }

  M5.dis.display.clear();
  
  if (gps.satellites.value() == 0) {
    M5.dis.display.drawRect(0, 0, 5, 5, RED);
    M5.dis.display.setCursor(10, 10);
    M5.dis.display.print("Satellites: X");
  } else {
    M5.dis.display.drawRect(0, 0, 5, 5, GREEN);
    M5.dis.display.setCursor(10, 10);
    M5.dis.display.printf("Lat: %.6f", gps.location.lat());
    M5.dis.display.setCursor(10, 20);
    M5.dis.display.printf("Lng: %.6f", gps.location.lng());
    M5.dis.display.setCursor(10, 30);
    M5.dis.display.printf("Ele: %.2f m", gps.altitude.meters());
    M5.dis.display.setCursor(10, 40);
    M5.dis.display.printf("Speed: %.2f km/h", gps.speed.kmph());
    M5.dis.display.setCursor(10, 50);
    M5.dis.display.printf("Batt: %d%%", M5.Axp.GetBatVoltage());
    M5.dis.display.setCursor(10, 60);
    M5.dis.display.printf("Files: %d", filesRecordedThisWeek);
  }

  M5.dis.display.display();
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  timeClient.update();
}

void bluetoothSetup() {
  SerialBT.onData([](const uint8_t *buffer, size_t size) {
    Serial.write(buffer, size);
  });
}
