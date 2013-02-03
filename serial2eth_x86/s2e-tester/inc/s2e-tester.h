 /* 
  *  s2e-tester.h
  *
  *  Created on: Jan 16, 2011
  *      Author: Cristian Axenie
  *     
  * Aplicatie de transport(conversie) a informatiei de pe interfata seriala pe o interfata UDP/IP Ethernet 
  * destinata comunicatiei cu module distribuite de control intr-un sistem SCADA.
  * 
  * Componenta SERVER - comunica cu modulele de comunicatie si primeste informatie impachetata UDP/IP pe suport  
  * Ethernet ce contine informatie de stare sau date de la senzorii procesului conectati la modulele IO.
  * Aplicatia ruleaza pe unitatatea centrala de monitorizare a ierarhiei SCADA. 
  *
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
#include <time.h>

/* Macrodefinitii si variabile simbolice */
#define PORT 3000
#define BUFLEN 60

// macro pentru logging
#define s2eLogMessage(format,args...) \
			do{ \
				    fprintf(stdout,format,args); \
			  } while(0);

// descriptor conexiune
int s;
/* Functie responsabila cu logarea erorilor, extragerea mesajelor de eroare din structurile aferente */
extern void s2eLogError(char *s);
/* Functie reponsabila pentru captarea semnalelor de tip SIGINT pentru oprirea serverului */
extern void s2eStopComm(int x);
