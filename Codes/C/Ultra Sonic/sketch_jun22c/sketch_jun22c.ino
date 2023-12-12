#include <avr/io.h>
#include <util/delay.h>

#define TRIGGER_PIN PD2    // Trigger pin connected to PD2 (INT0)
#define ECHO_PIN PD3       // Echo pin connected to PD3 (INT1)

volatile uint16_t pulseDuration = 0;
volatile uint8_t measurementComplete = 0;

void ultrasonicInit() {
    // Configure trigger pin as output and echo pin as input
    DDRD |= (1 << TRIGGER_PIN);
    DDRD &= ~(1 << ECHO_PIN);

    // Configure external interrupts for INT0 (trigger pin) and INT1 (echo pin)
    EICRA |= (1 << ISC01);  // Trigger INT0 on falling edge
    EICRA |= (1 << ISC11);  // Trigger INT1 on rising edge
    EIMSK |= (1 << INT0);   // Enable INT0 interrupt
    EIMSK |= (1 << INT1);   // Enable INT1 interrupt
}

void sendTriggerPulse() {
    // Generate a 10 us trigger pulse on trigger pin
    PORTD |= (1 << TRIGGER_PIN);
    _delay_us(10);
    PORTD &= ~(1 << TRIGGER_PIN);
}

float calculateDistance() {
    // Calculate distance in centimeters using the pulse duration
    float distance = pulseDuration * 0.0343 / 2.0;  // Speed of sound = 343 m/s
    return distance;
}

int main() {
    // Initialize ultrasonic sensor
    ultrasonicInit();

    // Enable global interrupts
    sei();

    while (1) {
        // Trigger ultrasonic sensor
        sendTriggerPulse();

        // Wait for the measurement to complete
        while (!measurementComplete);

        // Calculate and display the distance
        float distance_cm = calculateDistance();
        // You can use the distance_cm value as needed for further processing or display

        // Reset measurement complete flag for the next measurement
        measurementComplete = 0;

        // Delay before next measurement
        _delay_ms(1000);
    }

    return 0;
}

// Interrupt service routine for INT1 (echo pin)
ISR(INT1_vect) {
    if (PIND & (1 << ECHO_PIN)) {
        // Rising edge detected, start timer
        TCNT1 = 0;
    } else {
        // Falling edge detected, stop timer and calculate pulse duration
        pulseDuration = TCNT1;
        measurementComplete = 1;
    }
}
