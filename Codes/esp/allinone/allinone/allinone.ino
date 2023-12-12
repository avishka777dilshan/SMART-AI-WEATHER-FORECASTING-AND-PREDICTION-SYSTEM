#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <MQ135.h>
#include <DHT.h>

// Ultrasonic
const int trigPin = 4;   // D4 (GPIO4) as the trigger pin
const int echoPin = 5;   // D5 (GPIO5) as the echo pin
long duration;
int distance;

// DHT11
#define DHTPIN 2         // Pin connected to the DHT11 sensor (D4 / GPIO2)
#define DHTTYPE DHT11    // DHT sensor type
DHT dht(DHTPIN, DHTTYPE); // Initialize the DHT object

// Wi-Fi credentials
const char* ssid = "Avishka";
const char* password = "12345678";

// Firebase credentials
const char* firebaseHost = "allinone-b44b0-default-rtdb.firebaseio.com";
const char* firebaseAuth = "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x";

int uv;

// Create an instance of the BMP280 sensor
Adafruit_BMP280 bmp;  // Using the default I2C address 0x77

// Create an instance of the MQ135 sensor
MQ135 mq135(A0); // Replace A0 with the actual pin connected to the MQ135 sensor

// Create an instance of the FirebaseESP8266 class
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

  // Connect to Firebase
  Firebase.begin(firebaseHost, firebaseAuth);
  Firebase.reconnectWiFi(true);
  while (!Firebase.beginStream(firebaseData, "/")) {
    Serial.println("Could not connect to Firebase");
    delay(1000);
  }
  Serial.println("Connected to Firebase");

  // Initialize the BMP280 sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Ultrasonic
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // DHT11
  dht.begin();
}

void loop() {
  // Read UV Index using analog pin A0
  int sensorValue = analogRead(A0);
  int mv = sensorValue * 5000 / 1023.0;
  uv = UVIndex(mv);

  // Read distance using ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Read temperature and humidity from DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read pressure, altitude, and air quality from BMP280 and MQ135 sensors
  float pressure = bmp.readPressure() / 100.0;  // Pa to hPa conversion
  float altitude = bmp.readAltitude();
  int airQuality = mq135.getPPM();

  // Print sensor data to the serial monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" meters");
  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.println(" ppm");
  Serial.print("UV Index: ");
  Serial.println(uv);
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Update Firebase data
  Firebase.setFloat(firebaseData, "/temperature", temperature);
  Firebase.setFloat(firebaseData, "/humidity", humidity);
  Firebase.setFloat(firebaseData, "/pressure", pressure);
  Firebase.setFloat(firebaseData, "/altitude", altitude);
  Firebase.setInt(firebaseData, "/airQuality", airQuality);
  Firebase.setInt(firebaseData, "/uvIndex", uv);
  Firebase.setInt(firebaseData, "/distance", distance);

  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  }

  delay(5000); // Wait for 5 seconds before the next reading
}

int UVIndex(int v) {
  if (v < 50) {
    return 0;
  } else if (50 < v && v < 227) {
    return 1;
  } else if (227 < v && v < 318) {
    return 2;
  } else if (318 < v && v < 408) {
    return 3;
  } else if (408 < v && v < 503) {
    return 4;
  } else if (503 < v && v < 606) {
    return 5;
  } else if (606 < v && v < 696) {
    return 6;
  } else if (696 < v && v < 795) {
    return 7;
  } else if (795 < v && v < 881) {
    return 8;
  } else if (976 < v && v < 1079) {
    return 9;
  } else if (1079 < v && v < 1170) {
    return 10;
  } else if (v >= 1170) {
    return 11;
  }
  // Default case
  return -1;
}
