#include <avr/io.h>
#include <util/delay.h>

#define TRIG_PIN PB1
#define ECHO_PIN PB0

volatile unsigned long duration;
volatile unsigned int distance;

void UART_init() {
    UBRR0H = 0;
    UBRR0L = 103; // Baud rate 9600 at 16MHz
    UCSR0B = (1 << TXEN0); // Enable transmitter
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8 data bits, 1 stop bit
}

void UART_transmit(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))); // Wait for empty transmit buffer
    UDR0 = data;
}

void UART_printString(const char* str) {
    for (uint8_t i = 0; str[i] != '\0'; i++) {
        UART_transmit(str[i]);
    }
}

void setup() {
    // Set trigPin as output and echoPin as input
    DDRB |= (1 << TRIG_PIN);
    DDRB &= ~(1 << ECHO_PIN);

    UART_init();
}

void measureDistance() {
    // Send trigger signal
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    // Measure pulse duration
    TCNT1 = 0; // Reset timer
    TCCR1B = (1 << CS10); // Start timer with no prescaler

    while (!(PINB & (1 << ECHO_PIN))) {
        if (TCNT1 > 30000) // Exit if no echo received
            return;
    }

    while (PINB & (1 << ECHO_PIN)) {
        if (TCNT1 > 30000) // Exit if echo pulse too long
            return;
    }

    TCCR1B = 0; // Stop timer
    duration = TCNT1;

    // Calculate distance in cm
    distance = duration * 0.034 / 2;
}

void loop() {
    measureDistance();

    // Print distance on Serial Monitor
    char distanceStr[10];
    sprintf(distanceStr, "%d cm\n", distance);
    UART_printString(distanceStr);

    _delay_ms(500); // Delay between measurements
}

int main() {
    setup();

    while (1) {
        loop();
    }

    return 0;
}
