#include <avr/io.h>
#include <util/delay.h>

#define DHT_PIN PD2   // Pin connected to DHT11 data line

void delayMicroseconds(uint16_t us) {
    _delay_us(us);
}

void delayMilliseconds(uint16_t ms) {
    _delay_ms(ms);
}

void DHT_start() {
    // Set DHT_PIN as output
    DDRD |= (1 << DHT_PIN);
    
    // Send start signal to DHT11
    PORTD &= ~(1 << DHT_PIN);  // Set pin low
    delayMilliseconds(18);    // Hold low for at least 18ms
    PORTD |= (1 << DHT_PIN);   // Set pin high
    delayMicroseconds(30);     // Wait for DHT11 response
}

uint8_t DHT_checkResponse() {
    // Set DHT_PIN as input
    DDRD &= ~(1 << DHT_PIN);
    
    // Wait for DHT11 to respond
    uint8_t timeout = 100;
    while (!(PIND & (1 << DHT_PIN))) {
        delayMicroseconds(1);
        timeout--;
        if (timeout == 0) {
            return 0;  // Response timeout
        }
    }
    
    // Wait for DHT11 to finish response
    timeout = 100;
    while (PIND & (1 << DHT_PIN)) {
        delayMicroseconds(1);
        timeout--;
        if (timeout == 0) {
            return 0;  // Response timeout
        }
    }
    
    return 1;  // Response received
}

uint8_t DHT_readByte() {
    uint8_t byte = 0;
    
    for (uint8_t i = 0; i < 8; i++) {
        // Wait for falling edge (start of bit)
        uint8_t timeout = 100;
        while (!(PIND & (1 << DHT_PIN))) {
            delayMicroseconds(1);
            timeout--;
            if (timeout == 0) {
                return 0;  // Timeout
            }
        }
        
        // Wait for rising edge (end of bit)
        timeout = 100;
        while (PIND & (1 << DHT_PIN)) {
            delayMicroseconds(1);
            timeout--;
            if (timeout == 0) {
                return 0;  // Timeout
            }
        }
        
        // If high pulse duration > low pulse duration, set bit to 1
        if (timeout < 50) {
            byte |= (1 << (7 - i));
        }
    }
    
    return byte;
}

int main() {
    // Initialize UART
    UBRR0H = 0;
    UBRR0L = 103;   // Set baud rate to 9600
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Main loop
    while (1) {
        // Request data from DHT11
        DHT_start();
        
        // Check if DHT11 responds
        if (DHT_checkResponse()) {
            // Read data from DHT11
            uint8_t humidityHigh = DHT_readByte();
            uint8_t humidityLow = DHT_readByte();
            uint8_t temperatureHigh = DHT_readByte();
            uint8_t temperatureLow = DHT_readByte();
            uint8_t checksum = DHT_readByte();
            
            // Verify checksum
            if ((humidityHigh + humidityLow + temperatureHigh + temperatureLow) == checksum) {
                // Calculate temperature and humidity values
                float humidity = humidityHigh;
                float temperature = temperatureHigh;
                
                // Display temperature and humidity values
                char buffer[10];
                sprintf(buffer, "%.1f", temperature);
                for (uint8_t i = 0; buffer[i] != '\0'; i++) {
                    while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
                    UDR0 = buffer[i];
                }
                while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
                UDR0 = ' ';
                
                sprintf(buffer, "%.1f", humidity);
                for (uint8_t i = 0; buffer[i] != '\0'; i++) {
                    while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
                    UDR0 = buffer[i];
                }
                while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
                UDR0 = '\n';
            }
        }
        
        // Delay before taking the next measurement
        delayMilliseconds(2000);
    }
    
    return 0;
}
