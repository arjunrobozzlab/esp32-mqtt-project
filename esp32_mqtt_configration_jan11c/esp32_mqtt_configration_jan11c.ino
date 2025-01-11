#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// WiFi and MQTT settings
const char* ssid = "SMARTAXIOM";
const char* password = "Amit1305";

const char* mqtt_server = "gull.rmq.cloudamqp.com";
const char* mqtt_username = "ejumsfuq:ejumsfuq";
const char* mqtt_password = "23apT7-ha1RDMnhhjNOSPUYlCcXZeURj";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);


#define LED1_PIN 2
#define LED2_PIN 15
#define DHT_PIN 4
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
unsigned long interval = 10000; // Default interval in milliseconds
bool led1State = false;
bool led2State = false;

String getMacAddress() {
  return WiFi.macAddress();
}

String getTimestamp() {
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
  sprintf(timestamp, "%02d:%02d:%02d ",
          currentHour, currentMinute, currentSecond
          );

  return String(timestamp);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  timeClient.begin();
   timeClient.setTimeOffset(19800);  // IST offset of 5:30 hours in seconds (5.5 * 3600)

}

void callback(char* topic, byte* payload, unsigned int length) {
  String command = "";
  for (unsigned int i = 0; i < length; i++) {
    command += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(command);

  if (command.startsWith("@A0:B7:65:22:3C:64@$interval$~")) {
    int newInterval = command.substring(command.indexOf('~') + 1, command.lastIndexOf('~')).toInt();
    if (newInterval > 0) {
      interval = newInterval * 1000; // Convert seconds to milliseconds
      Serial.print("Interval updated to: ");
      Serial.println(interval);
    }
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$status$~Device1~OK")) {
    String status = led1State ? "ON" : "OFF";
    String response = "Device: " + getMacAddress() + ", Timestamp: " + getTimestamp() + ", LED1: " + status;
    client.publish("device/status", response.c_str());
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$status$~Device2~OK")) {
    String status = led2State ? "ON" : "OFF";
    String response = "Device: " + getMacAddress() + ", Timestamp: " + getTimestamp() + ", LED2: " + status;
    client.publish("device/status", response.c_str());
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$led1$~ON~OK")) {
    led1State = true;
    digitalWrite(LED1_PIN, HIGH);
    Serial.println("LED1 turned ON");
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$led1$~OFF~OK")) {
    led1State = false;
    digitalWrite(LED1_PIN, LOW);
    Serial.println("LED1 turned OFF");
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$led2$~ON~OK")) {
    led2State = true;
    digitalWrite(LED2_PIN, HIGH);
    Serial.println("LED2 turned ON");
  } else if (command.startsWith("@A0:B7:65:22:3C:64@$led2$~OFF~OK")) {
    led2State = false;
    digitalWrite(LED2_PIN, LOW);
    Serial.println("LED2 turned OFF");
  } else if (command.startsWith("@NVW1138@$restart$OK")) {
    ESP.restart();
  } else if (command.startsWith("@NVW1138@$ping$OK")) {
    String response = "Device: " + getMacAddress() + ", Timestamp: " + getTimestamp() + ", Sensor Data: 23.5C, Battery: 3.7V"; // Dummy data
    client.publish("device/ping", response.c_str());
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("device/commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > interval) {
    lastMsg = now;

    float h = random(15,35);
    float t = random(30,45);
    if (isnan(h) || isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    String payload = "Device: " + getMacAddress() + ", Timestamp: " + getTimestamp() + ", Temperature: " + String(t) + "C, Humidity: " + String(h) + "%";
    client.publish("sensor/data", payload.c_str());
  }
}
