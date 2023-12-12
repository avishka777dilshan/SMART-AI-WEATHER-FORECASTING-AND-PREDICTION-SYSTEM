void setup() {
  // Start serial communication
  Serial.begin(9600);
}

void loop() {
  // Count from 1 to 1000
  for (int count = 1; count <= 1000; ++count) {
    // Print the count to the Serial Monitor
    Serial.println(count);
    
    // Wait for 1 second
    delay(5000);
  }

  // Endless loop, will keep counting after reaching 1000
  while (true) {
    // Do nothing
  }
}
