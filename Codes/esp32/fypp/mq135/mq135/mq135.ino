#include <WiFi.h>
#include <FirebaseESP32.h>
#include <MQ135.h>

// Wi-Fi credentials
const char* ssid = "Avishka";
const char* password = "12345678";

// Firebase credentials
const char* firebaseHost = "allinone-b44b0-default-rtdb.firebaseio.com";
const char* firebaseAuth = "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x";

// MQ135 sensor
MQ135 mq135(39); // Connect MQ135 sensor to A0 pin

// Firebase data object
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
  while (!Firebase.beginStream(firebaseData, "/")) {
    Serial.println("Could not connect to Firebase");
    delay(1000);
  }
  Serial.println("Connected to Firebase");
}

void loop() {
  // Read air quality from MQ135 sensor
  int airQuality = mq135.getPPM();

  // Print air quality to serial monitor
  Serial.print("Air Quality: ");
  Serial.print(airQuality);
  Serial.println(" ppm");

  // Update Firebase data
  Firebase.setInt(firebaseData, "/airQuality", airQuality);
  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  }

  delay(5000); // Wait for 5 seconds before the next reading
}
