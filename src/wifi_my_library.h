
void startWiFi()
{
  Serial.println("Start connecting to wifi network");
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to Wifi was successful");
  Serial.println("IP address: " + WiFi.localIP().toString());
}

void checkWiFiConnect()
{
  if (WiFi.status() != WL_CONNECTED) {
    startWiFi();
  }
}
