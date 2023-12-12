#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

#define FIREBASE_HOST "allinone-b44b0-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x"
#define WIFI_SSID "Avishka"
#define WIFI_PASSWORD "12345678"

#define DHTPIN 4   // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11  // DHT sensor type

DHT dht(DHTPIN, DHTTYPE);

FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  dht.begin();
}

void loop() {
  delay(5000);

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C\t");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Firebase.setFloat(firebaseData, "/temperature", temperature);
  Firebase.setFloat(firebaseData, "/humidity", humidity);

  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  } else {
    Serial.println("Firebase data uploaded successfully");
  }
}
