
#include <WiFiS3.h>

#define SCAN_INTERVAL 3000
#define MAX_NETWORKS 10

struct Network {
  String bssid;
  String ssid;
  int previousRSSI;
  int currentRSSI;
  bool seen;
};

Network networks[MAX_NETWORKS];
int networkCount = 0;

unsigned long lastScan = 0;

String getBSSID(int index) {
  byte mac[6] = {0};

  if (WiFi.BSSID(index, mac) == nullptr) {
    return "";
  }

  char address[18];

  snprintf(address, sizeof(address),
    "%02X:%02X:%02X:%02X:%02X:%02X",
    mac[0], mac[1], mac[2],
    mac[3], mac[4], mac[5]);

  return String(address);
}

void scanWiFi() {

  for (int i = 0; i < networkCount; i++) {
    networks[i].seen = false;
  }

  int count = WiFi.scanNetworks();

  if (count < 0) {
    Serial.println("Scan failed");
    return;
  }

  Serial.println("\n===== SIGNAL CHANGES =====");

  for (int i = 0; i < count; i++) {

    String bssid = getBSSID(i);

    if (bssid.length() == 0) continue;

    int rssi = WiFi.RSSI(i);
    int found = -1;

    for (int j = 0; j < networkCount; j++) {
      if (networks[j].bssid == bssid) {
        found = j;
        break;
      }
    }

    if (found == -1) {

      if (networkCount >= MAX_NETWORKS) continue;

      found = networkCount++;

      networks[found].bssid = bssid;
      networks[found].ssid = WiFi.SSID(i);
      networks[found].previousRSSI = rssi;
      networks[found].currentRSSI = rssi;
      networks[found].seen = true;

      Serial.print("[NEW] ");
      Serial.print(networks[found].ssid);
      Serial.print(" | ");
      Serial.print(rssi);
      Serial.println(" dBm");

    } else {

      networks[found].previousRSSI =
        networks[found].currentRSSI;

      networks[found].currentRSSI = rssi;
      networks[found].seen = true;

      int change = rssi -
        networks[found].previousRSSI;

      Serial.print(networks[found].ssid);
      Serial.print(" | ");
      Serial.print(rssi);
      Serial.print(" dBm | ");

      if (change > 0) {
        Serial.print("STRONGER +");
        Serial.print(change);
        Serial.println(" dB");
      }
      else if (change < 0) {
        Serial.print("WEAKER ");
        Serial.print(change);
        Serial.println(" dB");
      }
      else {
        Serial.println("UNCHANGED");
      }
    }
  }

  Serial.println("==========================");
}

void setup() {

  Serial.begin(9600);

  unsigned long start = millis();

  while (!Serial && millis() - start < 5000) {
    delay(10);
  }

  Serial.println("WIFI SIGNAL CHANGE DETECTOR");

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module unavailable!");

    while (true) {
      delay(1000);
    }
  }
}

void loop() {

  if (millis() - lastScan >= SCAN_INTERVAL) {

    lastScan = millis();

    scanWiFi();
  }
}