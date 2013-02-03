/*
 *  s2e-tester.c
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
 */

#include "../inc/s2e-tester.h"

/* Functie responsabila cu logarea erorilor, extragerea mesajelor de eroare din structurile aferente */
void s2eLogError(char *s)
{
  perror(s);
  exit(EXIT_FAILURE);
}

/* Functie reponsabila pentru captarea semnalelor de tip SIGINT pentru oprirea serverului */

void s2eStopComm(int x)
{
  s2eLogMessage
      ("s2eTester : Serverul care capteaza pachete UDP/IP a primit semnal de stop!%s",
       "\n");
  close(s);
  s2eLogMessage("s2eTester : Conexiunea UDP/IP a fost inchisa !%s", "\n");
  s2eLogMessage
      ("s2eTester : ===================================================%s",
       "\n");
}


int main(void)
{
  struct sockaddr_in si_me, si_other;	// socketi pentru retinearea adreselor UDP
  int slen = sizeof(si_other);	// dimensiunea adresei
  char buf[BUFLEN];		// dimensiunea bufferelor RX/TX (2 char)
  char *data;
  s2eLogMessage
      ("s2eTester : ===================================================%s",
       "\n");
  s2eLogMessage
      ("s2eTester : Convertorul Serial la Ethernet a fost lansat !%s",
       "\n");
  s2eLogMessage
      ("s2eTester : Serverul care capteaza pachete UDP/IP a fost lansat !%s",
       "\n");
  s2eLogMessage
      ("s2eTester : Se primesc valori de la modulul IO distribuit : ....%s",
       "\n");
  // se creeaza un socket pentru ascultarea pe conexiunea UDP/IP                
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    s2eLogError("socket");
  // aloca memorie pentru membrii structurii IP si seteaza valori de initializare
  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);
  // se realizeaza bindingul la conexiune
  if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1)
    s2eLogError("bind");
  // se primesc pachetele
  while (1) {
    if (recvfrom
	(s, buf, sizeof(buf), 0, (struct sockaddr *) &si_other,
	 (socklen_t *) & slen) == -1)
      exit(EXIT_FAILURE);
    // se afiseaza informatia primita de la nodul distribuit de comunicatie cu modulul IO ( tensiunea   corespunzatoare de la iesire senzorului (intrarea modului IO distribuit))
    data = strtok(buf, "\n");
    // filtru pentru a filtra pachetele care contin doar informatie partiala
    // informatia partiala apare datorita faptului ca citirea din bufere  este asincrona
    if ((strstr(data, "rez") != NULL) && (strstr(data, "ADC") != NULL)
	&& (strstr(data, "U") != NULL) && (strstr(data, "V") != NULL)
	&& (strstr(data, "|") != NULL) && (strstr(data, "h") != NULL)
	&& (strstr(data, "val") != NULL)) {
      s2eLogMessage
	  ("s2eServer : Pachetele sunt receptionate de la modulul distribuit cu adresa %s:\n",
	   inet_ntoa(si_other.sin_addr));
      s2eLogMessage("s2eServer : Campul de date din pachet contine : %s\n",
		    data);
    }
    // sectiune in care se parseaza informatia primita si se determina tensiunea corespunzatoare de la iesirea
    // senzorului (intrarea modului IO distribuit)
    signal(SIGINT, s2eStopComm);
    signal(SIGKILL, s2eStopComm);
    signal(SIGHUP, s2eStopComm);
  }
  // se inchide conexiunea 
  close(s);
  return 0;
}
