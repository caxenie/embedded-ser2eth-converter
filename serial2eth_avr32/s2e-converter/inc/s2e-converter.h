/*
 *  s2e-converter.h
 *
 *  Created on: Jan 16, 2011
 *      Author: Cristian Axenie
 *
 * Aplicatie de transport(conversie) a informatiei de pe interfata seriala pe o interfata UDP/IP Ethernet 
 * destinata comunicatiei cu module distribuite de control intr-un sistem SCADA.
 * 
 * Componenta CLIENT - comunica cu modulele IO si trimite informatie impachetata UDP/IP pe suport Ethernet
 * unitatii centrale de monitorizare a ierarhiei SCADA	
 *
 * Fisier header
 *
 */
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

/* Macrodefinitii si variabile simbolice */
// dimensiunea maxima a bufferului
#define BUFLEN 512
#define NPACK 100

// macro pentru loggingul mesajelor de stare
unsigned char verbose = 0;
#define s2eLogMessage(format,args...) \
			do{ \
				if(verbose) \
				    fprintf(stdout,format,args); \
			  } while(0);


/* Prototipurile functiilor */
/* Functie responsabila cu logarea erorilor, extragerea mesajelor de eroare din structurile aferente */
extern void s2eLogError(char *s);
/* Functie care este responsabila cu setarea parametrilor de comunicatie pe linia seriala RS232 */
extern int s2ePortSetup(int s, struct termios *cfg, int speed, int data,
			unsigned char parity, int stopb);
/* Functie responsabila cu deschiderea portului serial pentru comunicatia RS232 */
extern int s2eOpenSerialPort(char *port);
/* Functie care implementeaza interfata de linie comanda pentru executia aplicatiei */
extern void s2eCommandLine();
/* Functie care implementeaza comportamentul la finalizarea comunicatiei. */
void s2eStopComm(int s);
