#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN PB5 // Pin 13 is connected to PB5

int main() {
    DDRB |= (1 << LED_PIN); // Set LED pin as output

    while (1) {
        PORTB ^= (1 << LED_PIN); // Toggle the LED pin
        _delay_ms(500); // Delay for 500 milliseconds
    }

    return 0;
}
