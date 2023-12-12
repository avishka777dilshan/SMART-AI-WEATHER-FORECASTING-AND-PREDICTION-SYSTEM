/*
 * wind_vane.ino
 */

#define WIND_VANE_IN_Channel A3

const int WEST = 1;
const int EAST = 2;
const int SOUTH = 3;
const int NORTH = 4;

uint16_t vane_reading = 0;

void setup() {
  pinMode(WIND_VANE_IN_Channel, INPUT);
  Serial.begin(9600);
}

void loop() {
  uint16_t rawReading = analogRead(WIND_VANE_IN_Channel);
  //Serial.print("Raw Reading: ");
  //Serial.println(rawReading);

  uint8_t windDirection = getWindDirection(rawReading);
  Serial.print("Wind Direction: ");
  switch (windDirection) {
    case WEST:
      Serial.println("West");
      break;
    case EAST:
      Serial.println("East");
      break;
    case SOUTH:
      Serial.println("South");
      break;
    case NORTH:
      Serial.println("North");
      break;
    default:
      Serial.println("Unknown");
  }
  delay(1000); // Adjust delay as needed for your application
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
