// uv,pressure,tmp,hue,alt,vane,speed
int uv;
String dir;
//-------------------bmp280-------------------------
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp; // Create BMP280 object
//-------------------------dht11---------------------------

#include "DHT.h"
#define DHT11PIN 5
DHT dht(DHT11PIN, DHT11);
//-----------------wind vane------------------------//

#define WIND_VANE_IN_Channel A3

const int WEST = 1;
const int EAST = 2;
const int SOUTH = 3;
const int NORTH = 4;

uint16_t vane_reading = 0;
//------------------wind speed-----------------------//
// Define the pin for the anemometer input
const int anemometerPin = 2; // Change this to the actual pin you're using
float windSpeed;

// Variables
volatile uint8_t count = 0;
volatile uint8_t sampleCounter = 0;
volatile uint32_t startTime = 0;
volatile uint32_t windTime = 0;
unsigned long lastTime = 0;
unsigned long interval = 1000; // Measurement interval in milliseconds
const int NUM_OF_SAMPLES = 50;
const int WIND_FACTOR = 2400;

//----------------lora trans-------------------------//
#include <LoRa.h>
#include <SPI.h>
 
#define ss A1
#define rst 9
#define dio0 3
//-----------------------------------------//
void setup() {
  Serial.begin(9600);
//---------------bmp280 i2c---------------//
    if (!bmp.begin(0x76)) {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1);
  }
//-------------------dht11----------------------//
 dht.begin();
//---------------------wind vane--------------------//
 pinMode(WIND_VANE_IN_Channel, INPUT);
 //-------------------wind speed----------------------//

 pinMode(anemometerPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(anemometerPin), anemometerISR, RISING);
  //-------------------lora trans----------------------//
while (!Serial);
  Serial.println("LoRa Sender");
 
  LoRa.setPins(ss, rst, dio0);    //setup LoRa transceiver module
  
  while (!LoRa.begin(433E6))     //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
delay(5000);
  //-----------------------------uv-------------------------------
  int sensorValue;
  int mv;

  sensorValue = analogRead(A2);
  //Serial.print("The voltage value: ");
  mv = sensorValue * 5000 / 4095.0;  // Adjusted for ESP32 ADC range
  //Serial.print(mv);
  //Serial.println("mV");
  //delay(1000);

  uv = UVIndex(mv);
  Serial.print("UV Index: ");
  Serial.println(uv);

  //delay(5000); // Wait for 5 seconds before the next reading
 //------------------------------------------------------------------------

 //-------------------------------bmp--------------------------------------
  //Serial.println("BMP280 Sensor Data:");

  // Read temperature
  //float temperature = bmp.readTemperature();
  //Serial.print("Temperature: ");
  //Serial.print(temperature);
  //Serial.println(" °C");

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

  

  //delay(2000); // Wait for 2 seconds before taking the next reading

  //------------------------------------dht11---------------------------------------
   float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.println("ºC ");
  Serial.print("Humidity: ");
  Serial.println(humi);
  
  //-------------------wind vane----------------------//
  uint16_t rawReading = analogRead(WIND_VANE_IN_Channel);
  //Serial.print("Raw Reading: ");
  //Serial.println(rawReading);

  uint8_t windDirection = getWindDirection(rawReading);
  Serial.print("Wind Direction: ");
  switch (windDirection) {
    case WEST:
      Serial.println("West");
      dir = "west";
      break;
    case EAST:
      Serial.println("East");
      dir="east";
      break;
    case SOUTH:
      Serial.println("South");
      dir="south";
      break;
    case NORTH:
      Serial.println("North");
      dir="north";
      break;
    default:
      Serial.println("Unknown");
  }
  //delay(1000); // Adjust delay as needed for your application
//----------------wind speed-------------------------//

//delay(5000);
  unsigned long currentTime = millis();

  // Calculate wind speed every second
  if (currentTime - lastTime >= interval) {
    // Calculate wind speed in meters per second
    windSpeed = getAverageWind();

    // Display the result
    Serial.print("Wind Speed: ");
    Serial.print(windSpeed);
    Serial.println(" m/s");

    // Reset variables
    windTime = 0;
    count = 0;
    sampleCounter = 0;
    lastTime = currentTime;
  }

 //-----------------lora trans------------------------//
 
  //Serial.print("Sending packet: ");
  //Serial.println(counter);
 String sensorDataString = String(uv)+","+String(pressure)+","+String(altitude)+","+String(temp)+","+String(humi)+","+String(windSpeed)+","+String(dir);
  LoRa.beginPacket();   //Send LoRa packet to receiver
  //LoRa.print("hello ");
  //LoRa.print(counter);
//  LoRa.print(uv);
//  LoRa.print(pressure);
//  LoRa.print(altitude);
//  LoRa.print(temp);
//  LoRa.print(humi);
//  LoRa.print(windSpeed);
  LoRa.print(sensorDataString);
  LoRa.endPacket();
 
//  counter++;
 
  //delay(10000);

 //-----------------------------------------//
 Serial.println();
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



uint8_t getWindDirection(uint16_t reading) {
//  Serial.print("Reading Range: ");
//  Serial.print("0-350: ");
//  Serial.print(reading >= 0 && reading <= 350);
//  Serial.print(", 350-580: ");
//  Serial.print(reading > 350 && reading <= 580);
//  Serial.print(", 580-800: ");
//  Serial.print(reading > 580 && reading <= 800);
//  Serial.print(", 800-1023: ");
//  Serial.println(reading > 800 && reading <= 1023);

  if (reading > 800 && reading <= 1023) {
    return WEST;
  } else if (reading > 580 && reading <= 800) {
    return EAST;
  } else if (reading > 350 && reading <= 580) {
    return SOUTH;
  } else if (reading >= 0 && reading <= 350) {
    return NORTH;
  } else {
    return 0;
  }
}


// Interrupt service routine to handle anemometer pulses
void anemometerISR() {
  //Serial.println("Pulse detected!");  // Debug print
  delay(1);
  if (1 == digitalRead(anemometerPin)) {
    if (count == 0) {
      startTime = micros();
    }
    count++;
    sampleCounter++;

    if (count == 2) {
      windTime = micros() - startTime;
      count = 0;
      startTime = 0;
    }

    if (NUM_OF_SAMPLES == sampleCounter) {
      Serial.print("Sample counter reached: ");  // Debug print
      Serial.println(sampleCounter);  // Debug print
      sampleCounter = 0;
    }
  }
}

// Function to calculate average wind speed
float getAverageWind() {
//  Serial.println("Calculating wind speed...");  // Debug print
//  Serial.print("windTime: ");
//  Serial.println(windTime);  // Debug print
//  Serial.print("sampleCounter: ");
//  Serial.println(sampleCounter);  // Debug print
  float tmp = WIND_FACTOR / (windTime / sampleCounter);
  //if (tmp > 50.0) {
  //  tmp = 0.0;
 // }
  windTime = 0;
  count = 0;
  sampleCounter = 0;
  return tmp;
}
