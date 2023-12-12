#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL   // Set the clock frequency to 16 MHz
#define BAUD_RATE 9600     // Desired baud rate

// Constants
#define TRIG_PIN PB1
#define ECHO_PIN PB0

// Function to initialize UART
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

// Function to transmit data over UART
void UART_transmit(uint8_t data) {
    // Wait for the transmit buffer to be empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Transmit the data
    UDR0 = data;
}

// Function to transmit a string over UART
void UART_transmitString(const char* str) {
    // Iterate through the string and transmit each character
    uint8_t i = 0;
    while (str[i] != '\0') {
        UART_transmit(str[i]);
        i++;
    }
}

// Function to generate a short pulse on TRIG pin
void triggerPulse() {
    PORTB |= (1 << TRIG_PIN);  // Set TRIG pin HIGH
    _delay_us(10);             // Wait for 10 microseconds
    PORTB &= ~(1 << TRIG_PIN); // Set TRIG pin LOW
}

// Function to measure distance using ultrasonic sensor
uint16_t measureDistance() {
    // Send trigger pulse
    triggerPulse();

    // Wait for the ECHO pin to go HIGH
    while (!(PINB & (1 << ECHO_PIN)));

    // Start Timer1 with prescaler 8
    TCCR1B |= (1 << CS11);
    TCNT1 = 0;

    // Wait for the ECHO pin to go LOW
    while (PINB & (1 << ECHO_PIN));

    // Stop Timer1
    TCCR1B = 0;

    // Calculate the pulse duration in microseconds
    uint16_t pulseDuration = (TCNT1 * 1000000UL) / F_CPU;

    // Calculate the distance in centimeters
    uint16_t distance = pulseDuration / 58;

    return distance;
}

int main() {
    // Initialize UART
    UART_init();

    // Set TRIG pin as output and ECHO pin as input
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);

    while (1) {
        // Measure the distance
        uint16_t distance = measureDistance();

        // Convert distance to a string
        char distanceStr[10];
        itoa(distance, distanceStr, 10);

        // Transmit the distance over UART
        UART_transmitString("Distance: ");
        UART_transmitString(distanceStr);
        UART_transmitString(" cm\r\n");

        // Delay before taking the next measurement
        _delay_ms(500);
    }

    return 0;
}
