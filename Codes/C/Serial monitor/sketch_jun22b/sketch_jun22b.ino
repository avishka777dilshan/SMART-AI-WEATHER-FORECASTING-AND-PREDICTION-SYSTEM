#include <avr/io.h>
#include <util/delay.h>

#define F_CPU 16000000UL   // Set the clock frequency to 16 MHz
#define BAUD_RATE 9600     // Desired baud rate

#define TRIGGER_PIN PB1    // Trigger pin connected to PD2 (INT0)
#define ECHO_PIN PB0       // Echo pin connected to PD3 (INT1)

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

int main() {
    UART_transmitString("cc");
    // Initialize UART
    UART_init();

    // Counter variable
    uint8_t distance_cm = 0;

    // Initialize ultrasonic sensor
    ultrasonicInit();

    // Enable global interrupts
    sei();
    UART_transmitString("dd");
    while (1) {
    UART_transmitString("ee");
       // Trigger ultrasonic sensor
        sendTriggerPulse();

        // Wait for the measurement to complete
        while (!measurementComplete);
        UART_transmitString("ii");
        
        // Calculate and display the distance
        float distance_cm = calculateDistance();
        // You can use the distance_cm value as needed for further processing or display

        // Reset measurement complete flag for the next measurement
        measurementComplete = 0;

        // Delay before next measurement
        _delay_ms(1000);
        
        // Convert the count to a string
        char buffer[10];
        itoa(distance_cm, buffer, 10);

        // Transmit the count as a string
        UART_transmitString(buffer);
        UART_transmit('\n'); // Add a line break

        // Increment the count
        //distance_cm++;

        _delay_ms(1000); // Delay for 1 second
    }

    return 0;
}
// Interrupt service routine for INT1 (echo pin)
ISR(INT1_vect) {
    UART_transmitString("ff");
    if (PIND & (1 << ECHO_PIN)) {
      UART_transmitString("gg");
        // Rising edge detected, start timer
        TCNT1 = 0;
    } else {
      UART_transmitString("hh");
        // Falling edge detected, stop timer and calculate pulse duration
        pulseDuration = TCNT1;
        measurementComplete = 1;
    }
}
