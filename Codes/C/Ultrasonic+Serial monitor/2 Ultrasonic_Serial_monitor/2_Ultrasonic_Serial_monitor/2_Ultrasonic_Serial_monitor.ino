#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>

#define F_CPU 16000000UL   // Set the clock frequency to 16 MHz
#define BAUD_RATE 9600     // Desired baud rate

#define TRIGGER_PIN PB1    // Trigger pin connected to PB1 (pin 9)
#define ECHO_PIN PB0      // Echo pin connected to PB0 (pin 8)

volatile uint32_t pulse_start = 0;
volatile uint32_t pulse_end = 0;
volatile uint8_t measurement_ready = 0;

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

void ultrasonic_init() {
    // Configure trigger pin as output and echo pin as input
    DDRB |= (1 << TRIGGER_PIN);
    DDRB &= ~(1 << ECHO_PIN);

    // Configure External Interrupt on Rising Edge
    EICRA |= (1 << ISC01) | (1 << ISC00);
    EIMSK |= (1 << INT0);

    // Configure Timer/Counter1 for pulse width measurement
    TCCR1B |= (1 << ICES1);     // Capture rising edge
    TIMSK1 |= (1 << ICIE1);     // Enable Input Capture Interrupt
    TCCR1B |= (1 << CS10);      // Start Timer/Counter1 with no prescaler
}

void ultrasonic_measure() {
    // Generate a 10us pulse on the trigger pin
    PORTB |= (1 << TRIGGER_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIGGER_PIN);

    // Reset measurement variables
    pulse_start = 0;
    pulse_end = 0;
    measurement_ready = 0;

    // Wait for measurement to complete
    while (!measurement_ready);

    // Calculate the distance based on the pulse width
    uint32_t pulse_width = pulse_end - pulse_start;
    uint32_t distance = (pulse_width * 343) / (2 * 1000); // Divide by 2 to account for round trip

    // Transmit distance as string
    char buffer[20];
    sprintf(buffer, "Distance: %lu cm\r\n", distance);
    UART_transmitString(buffer);
}

ISR(INT0_vect) {
    if (PIND & (1 << ECHO_PIN)) {
        // Rising edge detected, record the start time
        pulse_start = ICR1;
    } else {
        // Falling edge detected, record the end time
        pulse_end = ICR1;
        measurement_ready = 1;
    }
}

int main() {
    // Initialize UART
    UART_init();

    // Initialize ultrasonic sensor
    ultrasonic_init();

    // Enable global interrupts
    sei();

    while (1) {
        ultrasonic_measure();
        _delay_ms(1000); // Delay between measurements
    }

    return 0;
}
