/*
 * main.c
 *
 * 	Created on: Jan 16, 2011
 *      Author: Cristian Axenie
 *      Aplicatie care citeste periodic (temporizare timer 1s)intrarea analogica 0 si transmite valoarea
 *      preluata pe linia seriala RS232 si daca valoarea citita este mai mare THRES_UP LED0 este comandat
 *	iar daca valoarea citita este mai mica decat THRES_DOWN este comandat LED1.
 */

#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* functie de initializare a subsistemelor mcu */
void init();
/* functie de initializare a convertorului adc */
extern void init_adc(void);
/* functie de citire a unui canal al adc */
extern unsigned char read_adc(unsigned char);
/* functie de initializare a modulului usart */
extern void init_usart(void);
/* functie de transmitere a unui caracter pe linia seriala */
extern void send_char(char);
/* functie de conversie binar la ascii */
extern void convert_to_ascii(unsigned char);
/* functie care implementeaza un soft timer pt temporizare */
extern void soft_timer(void);
/* functie de initializare a timerului hardware */
extern void init_timer(void);
/* functie care decrementeaza timerul cu 10ms */
extern void decrement_10ms(void);
/* functie care converteste 2 octeti hex in binar */
extern unsigned char convert_to_bin(unsigned char, unsigned char);

extern int txflag;// flag receptie
extern unsigned char txdata; // date de transmis
extern unsigned char lonib, hinib; // nibble superior si inferior a datei de transmis
extern unsigned char timer_table[8];// tabele timer
extern unsigned int OCR1;// valoarea care se va scrie in registrul output compare a Timer1
extern short qtoc;// flag de intrerupere

static unsigned char THRES_UP = 0xD3; // prag superior la 4 V
static unsigned char THRES_DOWN = 0x35; // prag inferior la 1 V

int main(){

  unsigned char adc_val;// valoarea citita de la convertorul ADC
  init();// initializare
  init_usart();// init USART
  init_timer();// init timer
  init_adc();// init ADC
  timer_table[0] = 100;// seteaza temporizarea de 1s
  // bucla infinita
  while(1){
    soft_timer();// se porneste timerul software
    adc_val = read_adc(0);// se citeste intrarea analogica 0
    if(timer_table[1]==0){ //daca timerul a expirat 
      timer_table[0] = 100; // reinitializare timer
      convert_to_ascii(adc_val);// se converteste in caractere ASCII pentru TX pe seriala
	send_char('r');	
	send_char('e');
	send_char('z');
	send_char(0x20);
	send_char('A');
	send_char('D');
	send_char('C');
	send_char(':');
	// se transmite caracterul pentru nibbleul superior
        send_char(hinib);
        // se transmite caracterul pentru nibbleul inferior
	send_char(lonib);
	send_char('h');
	send_char('|');
	// se converteste valoarea in binar pentru a extrage valoarea ternsiunii 
	// corespunzatoare pentru valoarea hex citita
	// U[V] = adc_val[hex]/33[hex] (interpolare liniara)
	convert_to_ascii(convert_to_bin(hinib, lonib)/0x33);
	send_char('v');
	send_char('a');
	send_char('l');
	send_char(0x20);
	send_char('U');
	send_char(':');
	// valoare zecimala a tensiunii corespunzatoare
        send_char(lonib);
	send_char('V');
	send_char(0xA);// delimitator
    }
    // se testeaza pregurile
    if(adc_val >= THRES_UP) PORTB|=(1<<0);// se comanda LED0
    else PORTB&=~(1<<0);// LED0 stins

    if(adc_val <= THRES_DOWN) PORTB|=(1<<1);// se comanda LED1
    else PORTB&=~(1<<1);// LED1 stins
  };
  return 0;
}

void init(void)
{
  // Initializarea porturilor IO
  // Code Generated by the Configuration Wizard
  PORTA=0x00;
  DDRA=0x00;

  PORTB=0x00;
  DDRB=0xFF;

  PORTC=0x00;
  DDRC=0x00;

  PORTD=0x00;
  DDRD=0x00;

  // Timer/Counter 0 initialization
  // Clock source: System Clock
  // Clock value: Timer 0 Stopped
  // Mode: Normal top=FFh
  // OC0 output: Disconnected
  TCCR0=0x00;
  TCNT0=0x00;
  OCR0=0x00;
  // Timer/Counter 2 initialization
  // Clock source: System Clock
  // Clock value: Timer 2 Stopped
  // Mode: Normal top=FFh
  // OC2 output: Disconnected
  ASSR=0x00;
  TCCR2=0x00;
  TCNT2=0x00;
  OCR2=0x00;
  // External Interrupt(s) initialization
  // INT0: Off
  // INT1: Off
  // INT2: Off
  MCUCR=0x00;
  MCUCSR=0x00;
  // Timer(s)/Counter(s) Interrupt(s) initialization
  TIMSK=0x10;
  // Analog Comparator initialization
  // Analog Comparator: Off
  // Analog Comparator Input Capture by Timer/Counter 1: Off
  ACSR=0x80;
  SFIOR=0x00;
}
