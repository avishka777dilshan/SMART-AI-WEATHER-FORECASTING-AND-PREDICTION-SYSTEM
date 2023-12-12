int uv;

void setup() { 
  Serial.begin(9600);
}

void loop() {  
  int sensorValue;
  int mv;
  
  sensorValue = analogRead(A0);
  Serial.print("The voltage value: ");
  mv = sensorValue * 5000 / 1023.0;
  Serial.print(mv);
  Serial.println("mV");
  delay(1000);
  Serial.println();

  uv = UVIndex(mv);
  Serial.print("UV Index: ");
  Serial.println(uv);
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
