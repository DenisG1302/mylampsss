#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include <WiFiClientSecure.h>

const char* ssid = "Xiaomi_81A4";
const char* password = "9137864906";
const char* update_url = "https://denisg1302.github.io/mylampsss/Text.ino.bin";

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println("Connected to Wi-Fi");
}

void loop() {
  Serial.println("Checking for updates...");
  HTTPClient httpClient;
  WiFiClientSecure wifiClient;

  // Configure the WiFiClientSecure instance to not check the server's certificate (use with caution)
  wifiClient.setInsecure();

  httpClient.begin(wifiClient, update_url);
  httpClient.setUserAgent(F("ESP8266-http-Update"));
  httpClient.setTimeout(5000);
  t_httpUpdate_return ret = ESPhttpUpdate.update(httpClient);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }

  delay(60000);
}
