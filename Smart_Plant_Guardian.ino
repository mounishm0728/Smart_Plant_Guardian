#include <ESP8266WiFi.h>
#include <DHT.h>

// Pin Definitions
#define DHTPIN D4          // DHT11 data pin
#define DHTTYPE DHT11      // DHT sensor type
#define MOISTURE_PIN A0    // Soil moisture sensor pin
#define LDR_PIN D3         // Light sensor pin
#define BUZZER_PIN D2      // Buzzer pin

// Wi-Fi Credentials
const char* ssid = "Your_WiFi_SSID";       // Replace with your Wi-Fi SSID
const char* password = "Your_WiFi_Password"; // Replace with your Wi-Fi password

// ThingSpeak API
const char* server = "api.thingspeak.com";
const char* apiKey = "Your_ThingSpeak_API_Key"; // Replace with your ThingSpeak API Key

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize Sensors and Actuators
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Read Sensor Data
  float moisture = analogRead(MOISTURE_PIN); // Read soil moisture
  float light = digitalRead(LDR_PIN);       // Read light intensity
  float temperature = dht.readTemperature(); // Read temperature

  // Check for Sensor Errors
  if (isnan(temperature)) {
    Serial.println("Failed to read temperature!");
    return;
  }

  // Check Conditions and Send Alerts
  if (moisture < 500) { // Soil is dry
    digitalWrite(BUZZER_PIN, HIGH);
    sendAlert("Low Soil Moisture!");
  } else if (light == LOW) { // Insufficient light
    digitalWrite(BUZZER_PIN, HIGH);
    sendAlert("Insufficient Light!");
  } else if (temperature > 35 || temperature < 10) { // Extreme temperature
    digitalWrite(BUZZER_PIN, HIGH);
    sendAlert("Extreme Temperature!");
  } else {
    digitalWrite(BUZZER_PIN, LOW); // Turn off buzzer if conditions are normal
  }

  delay(5000); // Wait 5 seconds before next reading
}

// Function to Send Alerts to ThingSpeak
void sendAlert(String message) {
  WiFiClient client;
  if (client.connect(server, 80)) {
    String url = "/update?api_key=" + String(apiKey) + "&field1=" + String(message);
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
    delay(10);
    client.stop();
  }
}
