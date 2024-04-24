/*
 * Lab6.c
 *
 * Created: 23/04/2024 18:57:35
 * Author : luisd
 */ 
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "ADC/ADC.h"

void initUART9600(void);
void writeUART(char caracter);
void writeTextUART(char* texto);

volatile char leds;
volatile char opcion;
volatile char valor;
volatile uint8_t menu = 0;

int main(void)
{
	DDRD |= 0xFF;
	DDRB |= 0x3F;
    initUART9600();
	init_ADC(0, 0, 128);
	sei();
	
	writeUART('H');
	writeUART('o');
	writeUART('l');
	writeUART('a');
	writeUART(' ');
	writeUART('m');
	writeUART('u');
	writeUART('n');
	writeUART('d');
	writeUART('o');
	writeUART('\n');
	writeUART('\n');
	
	//writeTextUART("Hola mundo xd");
	
	writeTextUART("Presione 1 para recibir caracter\n");
	writeTextUART("Presione 2 para enviar un caracter\n");
	
    while (1) 
    {
		if (menu == 1){
			valor = readADC(4);
			writeTextUART("Se recibió el siguiente caracter: ");
			while (!(UCSR0A & (1<<UDRE0)));
			UDR0 = valor;
			writeUART('\n');
			writeTextUART("Presione 1 para recibir caracter\n");
			writeTextUART("Presione 2 para enviar un caracter\n");
			menu = 0;
		}
    }
}

void initUART9600(void){
	DDRD &= ~(1 << DDD0);
	DDRD |= (1 << DDD1);
	
	UCSR0A = 0;
	UCSR0A |= (1 << U2X0);
	
	UCSR0B = 0;
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	UCSR0C = 0;
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	
	UBRR0 = 207;
}

void writeUART(char caracter){
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = caracter;
}

void writeTextUART(char* texto){
	uint8_t i;
	for(i = 0; texto[i] != '\0'; i++){
		while (!(UCSR0A & (1<<UDRE0)));
		UDR0 = texto[i];
	}
}

ISR(USART_RX_vect){
	opcion = UDR0;
	writeTextUART("--------------\n");
	while (!(UCSR0A & (1<<UDRE0)));
	UDR0 = opcion;
	writeTextUART("\n--------------\n");
	if (opcion == '1'){
		menu = 1;
		writeTextUART("Eligió la opción 1\n");
	} else if (opcion == '2') {	
		menu = 2;
		writeTextUART("Eligió la opción 2\n");
		writeTextUART("Envie el caracter que quiere mostrar en las leds\n");
	} else {
		if (menu == 2){
			writeTextUART("Se muestra el caracter: ");
			leds = opcion;
			while (!(UCSR0A & (1<<UDRE0)));
			PORTD = 0;
			PORTB = 0;
			PORTD |= (leds << 5) & 0xFF;
			PORTB |= (leds >> 3) & 0x3F;
			UDR0 = leds;
			writeUART('\n');
			writeTextUART("Presione 1 para recibir caracter\n");
			writeTextUART("Presione 2 para enviar un caracter\n");
			menu = 0;
		} else {
			writeTextUART("Opción no válida\n");
			writeTextUART("Presione 1 para recibir caracter\n");
			writeTextUART("Presione 2 para enviar un caracter\n");
		}
	}
}