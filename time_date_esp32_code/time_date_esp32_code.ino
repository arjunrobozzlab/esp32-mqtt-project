#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <TimeLib.h> // Include TimeLib.h for time functions
const char* ssid = "SMARTAXIOM";
const char* password = "Amit1305";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize NTPClient to get time
  timeClient.begin();

  // Set offset time in seconds to adjust for your timezone
  timeClient.setTimeOffset(19800); // IST: UTC +5:30 = 19800 seconds
}

void loop() {
  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  // Extract time components
  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  int currentSecond = timeClient.getSeconds();

  // Extract date components
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int currentYear = ptm->tm_year + 1900; // Full year
  int currentMonth = ptm->tm_mon + 1;    // Months are 0-11, so add 1
  int currentDay = ptm->tm_mday;

  // Create timestamp string
  char timestamp[25];
  sprintf(timestamp, "%02d:%02d:%02d %02d/%02d/%04d",
          currentHour, currentMinute, currentSecond,
          currentDay, currentMonth, currentYear);

  // Print the timestamp
  Serial.println("Timestamp: " + String(timestamp));

  delay(1000); // Update every second
}
