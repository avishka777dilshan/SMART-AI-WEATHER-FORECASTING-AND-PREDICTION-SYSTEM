#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Wi-Fi credentials
const char* ssid = "Avishka";
const char* password = "12345678";

// Firebase credentials
const char* firebaseHost = "allinone-b44b0-default-rtdb.firebaseio.com";
const char* firebaseAuth = "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x";

// Create an instance of the BMP280 sensor
Adafruit_BMP280 bmp;

// Create an instance of the FirebaseESP32 class
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize the BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Connect to Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
  Firebase.reconnectWiFi(true);
  while (!Firebase.beginStream(firebaseData, "/")) {
    Serial.println("Could not connect to Firebase");
    delay(1000);
  }
  Serial.println("Connected to Firebase");
}

void loop() {
  // Read temperature, pressure, and altitude from the BMP280 sensor
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;  // Pa to hPa conversion
  float altitude = bmp.readAltitude();

  // Print sensor data to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" meters");

  // Update Firebase data
  Firebase.setFloat(firebaseData, "/temperature", temp);
  Firebase.setFloat(firebaseData, "/pressure", pressure);
  Firebase.setFloat(firebaseData, "/altitude", altitude);

  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  }

  delay(5000); // Wait for 5 seconds before the next reading
}
