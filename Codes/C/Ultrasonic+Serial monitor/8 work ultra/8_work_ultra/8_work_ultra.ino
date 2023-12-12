#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL   // Set the clock frequency to 16 MHz
#define BAUD_RATE 9600     // Desired baud rate

// Constants
#define TRIG_PIN PB2
#define ECHO_PIN PB3

// Global variables
volatile uint16_t duration;
volatile uint16_t distance;

// Initialize UART
void UART_init() {
    // Calculate the UBRR value
    uint16_t ubrr = F_CPU / 16 / BAUD_RATE - 1;

    // Set the baud rate registers
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;

    // Enable transmitter and receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

// Transmit a byte via UART
void UART_transmit(uint8_t data) {
    // Wait for the transmit buffer to be empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Transmit the data
    UDR0 = data;
}

// Transmit a string via UART
void UART_transmitString(const char* str) {
    // Iterate through the string and transmit each character
    uint8_t i = 0;
    while (str[i] != '\0') {
        UART_transmit(str[i]);
        i++;
    }
}

// Measure the distance using the ultrasonic sensor
void measureDistance() {
    // Clear the trigger pin
    PORTB &= ~(1 << TRIG_PIN);
    _delay_us(2);

    // Set the trigger pin high for 10 microseconds
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    // Measure the pulse duration
    while (!(PINB & (1 << ECHO_PIN)));
    TCNT1 = 0; // Reset the counter
    TCCR1B |= (1 << CS10); // Start the timer

    while (PINB & (1 << ECHO_PIN));
    TCCR1B = 0; // Stop the timer
    duration = TCNT1;

    // Calculate the distance in centimeters
    distance = duration * 0.034 / 2;
}

int main() {
    // Initialize UART
    UART_init();

    // Set trigger pin as output and echo pin as input
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);

    // Enable internal pull-up for echo pin
    PORTB |= (1 << ECHO_PIN);

    // Main loop
    while (1) {
        // Measure the distance
        measureDistance();

        // Convert the distance to a string
        char distanceStr[16];
        snprintf(distanceStr, sizeof(distanceStr), "Distance: %d cm\n", distance);

        // Transmit the distance string via UART
        UART_transmitString(distanceStr);

        _delay_ms(500);
    }

    return 0;
}
