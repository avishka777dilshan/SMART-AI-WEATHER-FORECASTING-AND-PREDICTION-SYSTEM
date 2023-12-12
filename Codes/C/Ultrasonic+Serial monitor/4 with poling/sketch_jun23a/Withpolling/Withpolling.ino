#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL   // Set the clock frequency to 16 MHz
#define BAUD_RATE 9600     // Desired baud rate

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

void UART_transmit(uint8_t data) {
    // Wait for the transmit buffer to be empty
    while (!(UCSR0A & (1 << UDRE0)));

    // Transmit the data
    UDR0 = data;
}

void UART_transmitString(const char* str) {
    // Iterate through the string and transmit each character
    uint8_t i = 0;
    while (str[i] != '\0') {
        UART_transmit(str[i]);
        i++;
    }
}

void ultrasonic_trigger() {
    // Set the trigger pin (e.g., PB1) as output
    DDRB |= (1 << PB1);

    // Generate a 10 us pulse on the trigger pin
    PORTB |= (1 << PB1);
    _delay_us(10);
    PORTB &= ~(1 << PB1);
}

uint16_t ultrasonic_measure() {
    // Trigger the ultrasonic sensor
    ultrasonic_trigger();

    // Measure the duration of the pulse using a loop
    uint16_t pulse_duration = 0;
    while (!(PINB & (1 << PB0))) {
        pulse_duration++;
        _delay_us(1);
    }

    // Convert the pulse duration to distance in centimeters
    // Distance = (pulse_duration / 2) * (speed of sound in air / time per cm)
    uint16_t distance_cm = pulse_duration / 58;

    return distance_cm;
}

int main() {
    // Initialize UART
    UART_init();

    while (1) {
        // Measure the distance using the ultrasonic sensor
        uint16_t distance = ultrasonic_measure();

        // Convert the distance to a string
        char buffer[10];
        itoa(distance, buffer, 10);

        // Transmit the distance as a string
        UART_transmitString("Distance: ");
        UART_transmitString(buffer);
        UART_transmitString(" cm");
        UART_transmit('\n'); // Add a line break

        _delay_ms(1000); // Delay for 1 second
    }

    return 0;
}
