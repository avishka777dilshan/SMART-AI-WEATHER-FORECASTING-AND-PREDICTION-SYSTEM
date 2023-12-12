#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup() {
  Serial.begin(9600);
  
  while (!Serial);  
  Serial.println("LoRa Sender");

  if (!LoRa.begin(433E6)) { // or 915E6, the MHz speed of your module
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // Increment the counter
  counter++;

  // Send the counter value
  LoRa.beginPacket();
  LoRa.print(counter);
  LoRa.endPacket();

  Serial.print("Sent counter value: ");
  Serial.println(counter);

  delay(1000); // Adjust the delay as needed
}
