#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Set your Firebase project credentials
#define FIREBASE_HOST "bmp280-d4c8e-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "iHqTL1vYjRqHP2hSF0YbtgzJuQPLyjODMgXUGiuC"

// Create an instance of the BMP280 sensor
Adafruit_BMP280 bmp;

// Create an instance of the FirebaseESP8266 class
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  // Initialize the BMP280 sensor
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Connect to Wi-Fi
  WiFi.begin("Avishka", "12345678");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  while (!Firebase.beginStream(firebaseData, "/")) {
    Serial.println("Could not connect to Firebase");
    delay(1000);
  }
  Serial.println("Connected to Firebase");
}

void loop() {
  // Read temperature, pressure, and altitude from the BMP280 sensor
  float temperature = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0;  // Pa to hPa conversion
  float altitude = bmp.readAltitude();

  // Update Firebase data
  Firebase.setFloat(firebaseData, "/temperature", temperature);
  Firebase.setFloat(firebaseData, "/pressure", pressure);
  Firebase.setFloat(firebaseData, "/altitude", altitude);

  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  }

  // Print sensor data
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" meters");

  delay(5000); // Wait for 5 seconds before the next reading
}
