#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bmp; // Create BMP280 object

void setup() {
  Serial.begin(9600);
  
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
  
}

void loop() {
  Serial.println("BMP280 Sensor Data:");

  // Read temperature
  float temperature = bmp.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  // Read pressure
  float pressure = bmp.readPressure() / 100.0F; // Convert Pa to hPa
  Serial.print("Pressure: ");
  Serial.print(pressure);
  Serial.println(" hPa");

  // Read altitude
  float altitude = bmp.readAltitude(SEALEVELPRESSURE_HPA);
  Serial.print("Altitude: ");
  Serial.print(altitude);
  Serial.println(" meters");

  Serial.println();

  delay(2000); // Wait for 2 seconds before taking the next reading
}
