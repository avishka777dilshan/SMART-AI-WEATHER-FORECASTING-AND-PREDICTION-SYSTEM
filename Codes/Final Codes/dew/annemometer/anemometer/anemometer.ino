// Define the pin for the anemometer input
const int anemometerPin = 2; // Change this to the actual pin you're using

// Variables
volatile uint8_t count = 0;
volatile uint8_t sampleCounter = 0;
volatile uint32_t startTime = 0;
volatile uint32_t windTime = 0;
unsigned long lastTime = 0;
unsigned long interval = 1000; // Measurement interval in milliseconds
const int NUM_OF_SAMPLES = 50;
const int WIND_FACTOR = 2400;

void setup() {
  Serial.begin(9600);
  pinMode(anemometerPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(anemometerPin), anemometerISR, RISING);
}

void loop() {
  delay(5000);
  unsigned long currentTime = millis();

  // Calculate wind speed every second
  if (currentTime - lastTime >= interval) {
    // Calculate wind speed in meters per second
    float windSpeed = getAverageWind();

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
  if (tmp > 50.0) {
    tmp = 0.0;
  }
  windTime = 0;
  count = 0;
  sampleCounter = 0;
  return tmp;
}
