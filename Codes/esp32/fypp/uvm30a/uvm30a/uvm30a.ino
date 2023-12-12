//#include <WiFi.h>
//#include <FirebaseESP32.h>
//
//// Wi-Fi credentials
//const char* ssid = "Avishka";
//const char* password = "12345678";
//
//// Firebase credentials
//const char* firebaseHost = "allinone-b44b0-default-rtdb.firebaseio.com";
//const char* firebaseAuth = "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x";

int uv;

// Firebase data object
//FirebaseData firebaseData;

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi
//  WiFi.begin(ssid, password);
//  while (WiFi.status() != WL_CONNECTED) {
//    delay(1000);
//    Serial.println("Connecting to WiFi...");
//  }
//  Serial.println("Connected to WiFi");
//
//  // Initialize Firebase
//  Firebase.begin(firebaseHost, firebaseAuth);
//  Firebase.reconnectWiFi(true);
//  while (!Firebase.beginStream(firebaseData, "/")) {
//    Serial.println("Could not connect to Firebase");
//    delay(1000);
//  }
//  Serial.println("Connected to Firebase");
}

void loop() {
  int sensorValue;
  int mv;

  sensorValue = analogRead(A0);
  Serial.print("The voltage value: ");
  mv = sensorValue * 5000 / 4095.0;  // Adjusted for ESP32 ADC range
  Serial.print(mv);
  Serial.println("mV");
  delay(1000);

  uv = UVIndex(mv);
  Serial.print("UV Index: ");
  Serial.println(uv);

  // Update Firebase data
//  Firebase.setInt(firebaseData, "/UVIndex", uv);
//  if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
//    Serial.println("Firebase data upload failed");
//  }

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
