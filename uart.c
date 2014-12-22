#include <avr/io.h>
#include <stdio.h>


void uart_init(void);
 
// a. Deklaration der primitiven Ausgabefunktion
int uart_putchar(char c, FILE *stream);
 
// b. Umleiten der Standardausgabe stdout (Teil 1)
static FILE mystdout = FDEV_SETUP_STREAM( uart_putchar, NULL, _FDEV_SETUP_WRITE );
 
// c. Definition der Ausgabefunktion
int uart_putchar( char c, FILE *stream )
{
    if( c == '\n' )
        uart_putchar( '\r', stream );
 
    loop_until_bit_is_set( UCSR0A, UDRE0 );
    UDR0 = c;
    return 0;
}
 
void uart_init(void)
{
    UCSR0B |= (1<<TXEN0) | (1<<RXEN0);    // UART TX und RX einschalten
    UCSR0C |= (1<<UCSZ01)|(3<<UCSZ00);    // Asynchron 8N1 
 
    UBRR0H = 0; 
    UBRR0L = 8; //115200 @ 16MHZ 
  stdout = &mystdout;
}
 
