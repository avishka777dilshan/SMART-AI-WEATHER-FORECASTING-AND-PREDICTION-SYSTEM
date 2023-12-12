#include <LoRa.h>
#include <SPI.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
FirebaseData firebaseData;

#define ss 5
#define rst 14
#define dio0 2
int UV;
float pressure,altitude,temperature,humidity,windSpeed,fake;
String dir;
// Wi-Fi credentials
const char* ssid = "Avishka";
const char* password = "12345678";

// Firebase credentials
const char* firebaseHost = "allinone-b44b0-default-rtdb.firebaseio.com";
const char* firebaseAuth = "3HfPtW9FeVt1KUXXpHQopPKC9YE7Jk2llLgDts5x";

void setup() 
{
  Serial.begin(115200);
  
  //lora
  while (!Serial);
  Serial.println("LoRa Receiver");
 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
 
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");

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
 
void loop() 
{
  int packetSize = LoRa.parsePacket();    // try to parse packet
  if (packetSize) 
  {
    
    //Serial.print("Received packet '");
    String receivedDataString = "";
    while (LoRa.available())              // read packet
    {
//      String LoRaData = LoRa.readString();
//      Serial.print(LoRaData); 
        receivedDataString += (char)LoRa.read();
    }
//    Serial.print("' with RSSI ");         // print RSSI of packet
//    Serial.println(LoRa.packetRssi());

//    float temperature = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
//    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);
//
//    float humidity = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
//    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);
//
//    int lightLevel = receivedDataString.toInt();

//''''''''''''''''''''''''''''
    UV = receivedDataString.substring(0, receivedDataString.indexOf(',')).toInt();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    pressure = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    altitude = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    temperature = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    humidity = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    windSpeed = receivedDataString.substring(0, receivedDataString.indexOf(',')).toFloat();
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    dir = receivedDataString.substring(0, receivedDataString.indexOf(','));
    receivedDataString.remove(0, receivedDataString.indexOf(',') + 1);

    //fake = receivedDataString.toFloat();

  Serial.print("UV index : ");
  Serial.println(UV);
  Serial.print("pressure : ");
  Serial.println(pressure);
  Serial.print("altitude : ");
  Serial.println(altitude);
  Serial.print("temp : ");
  Serial.println(temperature);
  Serial.print("humi : ");
  Serial.println(humidity);
  Serial.print("windSpeed : ");
  Serial.println(windSpeed);
  Serial.print("wind direction : ");
  Serial.println(dir);
  Serial.println();

  Firebase.setInt(firebaseData, "/UV", UV);
  Firebase.setFloat(firebaseData, "/pressure", pressure);
  Firebase.setFloat(firebaseData, "/altitude", altitude);
  Firebase.setFloat(firebaseData, "/temp", temperature);
  Firebase.setFloat(firebaseData, "/humi", humidity);
  Firebase.setFloat(firebaseData, "/windSpeed", windSpeed);
  Firebase.setString(firebaseData, "/winddirection", dir);

if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
    Serial.println("Firebase data upload failed");
  
  }
  // Update Firebase data
//  Firebase.setFloat(firebaseData, "/UV", UV);
//  Firebase.setFloat(firebaseData, "/pressure", pressure);
//  Firebase.setFloat(firebaseData, "/altitude", altitude);
//  Firebase.setInt(firebaseData, "/temp", temperature);
//  Firebase.setInt(firebaseData, "/humi", humidity);
//  Firebase.setInt(firebaseData, "/windSpeed", windSpeed);

  //if (firebaseData.dataAvailable() && firebaseData.dataType() == "null") {
   // Serial.println("Firebase data upload failed");
  }
}
