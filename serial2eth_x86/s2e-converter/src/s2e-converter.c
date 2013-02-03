/*
 *  s2e-converter.c
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
 */
#include "../inc/s2e-converter.h"

/* 
Functie care este responsabila cu interfata de linie comanda utila la apelarea aplicatiei de 
conversie RS232 la ethernet.
*/
void s2eCommandLine()
{
  fprintf(stdout,
	  "Utilizare: s2e-converter -a <addr> <port> -d <dev> <opt> [--verbose | v] \n"
	  "unde <addr> si <dev> reprezinta argumente pentru:\n"
	  "  -a <adr> <port>          adresa serverului si port UDP\n"
	  "  -d <dev> <opt>           dispozitivul serial local si parametri de comunicatie\n"
	  " <opt> este de forma sss-dps\n"
	  "    sss este viteza de comunicatie seriala (50,..,230400)\n"
	  "     d numarul de biti per caracter (5,6,7,8)\n"
	  "     p tipul de paritate (N,E,O)\n"
	  "     s numarul de biti de stop (1,2)\n");
  return;
}

/* 
 * Functie responsabila cu deschiderea portului serial pentru comunicatia RS232. 
 * Returneaza file descriptorul asociat dispozitivului tty sau -1 in cazul unei erori.
 */

int s2eOpenSerialPort(char *port)
{
  int fd;			// file descriptor pentru port
  // se deschide portul ttyS0 cu urmatoarele moduri de acces
  // O_RDWR - deschidere pentru citire si scriere
  // O_NOCTTY  - dispozitivul terminal nu va controla procesrul curent
  // O_NDELAY - parametru intarziere
  fd = open(port, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {

    s2eLogMessage("s2eOpenSerialPort: Nu se poate deschide %s !\n", port);
  } else
    // functie file control 
    // seteaza flagurile de stare ale fisierului dat de un file descriptor      
    fcntl(fd, F_SETFL, 0);

  return (fd);
}

/* Functie responsabila cu logarea erorilor, extragerea mesajelor de eroare din structurile aferente. */

void s2eLogError(char *s)
{
  perror(s);
  exit(EXIT_FAILURE);
}

/*
 * Functie care implementeaza comportamentul la finalizarea comunicatiei.
 * - se elibereaza buferele de TX si RX si se elibereaza conexiunea seriala.
 */
void s2eStopComm(int s)
{
  // elibereaza datele scrise in bufferul de TX dar care nu au fost transmise
  // si datele care au fost receptionate in bufferul de RX dar nu au fost citite
  // pentru conexiunea 0
  tcflush(s, TCIOFLUSH);
  // se inchide conexiunea seriala
  close(s);

  // redirecteaza mesajele catre fluxul setat 
  s2eLogMessage("s2eStopComm: Conexiunea %d seriala fost inchisa !\n", s);
  exit(EXIT_SUCCESS);
}

/*
Functie care este responsabila cu setarea parametrilor de comunicatie pe linia seriala RS232

- seteaza viteza de comunicatie la transmisie si receptie
- seteaza reprezentarea caracterelor in informatia transmisa (5/6/7/8 biti / caracter)
- seteaza paritatea (N - none, E - even, O - odd) utila in detectia erorilor de transmisie
- seteaza numarul de biti de stop

Informatia privind parametrii de transmisie se seteaza intr-o instanta a structurii struct termios 
unde se incapsuleaza informatie privind parametrii pentru intrare, iesire, moduri de control,
moduri locale si caractere de control.
*/

int s2ePortSetup(int s, struct termios *cfg, int speed, int data,
		 unsigned char parity, int stopb)
{
  // se initializeaza instanta de configurare la valorile specifice OS
  tcgetattr(s, cfg);
  s2eLogMessage
      ("s2ePortSetup : Atributele de configurare au fost setate pentru conexiunea %d ...\n",
       s);
  // seteaza instanta de configurare a portului serial la valori default ale atributelor
  cfmakeraw(cfg);
  s2eLogMessage
      ("s2ePortSetup : S-a creat obiectul de configurare pentru conexiunea %d ...\n",
       s);
  // retine baudrate-ul 
  speed_t baudrate;
  switch (speed) {		// preia viteza
  case 9600:
    baudrate = B9600;
    s2eLogMessage("s2ePortSetup : S-a setat baudarate %d ...\n", speed);
    break;
  case 19200:
    baudrate = B19200;
    s2eLogMessage("s2ePortSetup : S-a setat baudarate %d ...\n", speed);
    break;
  case 38400:
    baudrate = B38400;
    s2eLogMessage("s2ePortSetup : S-a setat baudarate %d ...\n", speed);
    break;
  case 57600:
    baudrate = B57600;
    s2eLogMessage("s2ePortSetup : S-a setat baudarate %d ...\n", speed);
    break;
  case 115200:
    baudrate = B115200;
    s2eLogMessage("s2ePortSetup : S-a setat baudarate %d ...\n", speed);
    break;
  default:
    s2eLogMessage("s2ePortSetup : Viteza %d baud nu este suportata !\n",
		  speed);
    break;
  }

  // scrie in instanta structurii termios cfg valoarea de configurare 
  // pentru viteza pe RX din variabila baudrate                    
  cfsetispeed(cfg, baudrate);
  s2eLogMessage
      ("s2ePortSetup : S-a configurat conexiunea pentru RX pentru baudarate %d ...\n",
       speed);
  // scrie in instanta structurii termios cfg valoarea de configurare 
  // pentru viteza pe TX din variabila baudrate
  cfsetospeed(cfg, baudrate);
  s2eLogMessage
      ("s2ePortSetup : S-a configurat conexiunea pentru TX pentru baudarate %d ...\n",
       speed);

  // preia paritatea si seteaza campul corespunzator din structura de configurare

  switch (parity | 32) {
  case 'n':{			//  fara valoare de paritate
      // dezactiveaza generarea valorii de paritatae la TX
      // si verificarea paritarii la RX  
      cfg->c_cflag &= ~PARENB;
      s2eLogMessage
	  ("s2ePortSetup : Nu se genreaza bit de paritate pentru conexiunea %d\n",
	   s);
      break;
    }
  case 'e':			// valoare de paritate para 
    {
      // activeaza generarea valorii de paritate la TX si verificare la RX
      cfg->c_cflag |= PARENB;
      // activeaza verificarea paritatii la TX si RX pt valoare para
      cfg->c_cflag &= ~PARODD;
      s2eLogMessage
	  ("s2ePortSetup : Activeaza verificarea paritatii la TX si RX pt valoare para pentru conexiunea %d\n",
	   s);
      break;
    }
  case 'o':			// valoare de paritate impara
    {
      // activeaza generarea valorii de paritate la TX si verificare la RX
      cfg->c_cflag |= PARENB;
      // activeaza verificarea paritatii la TX si RX pt valoare impara 
      cfg->c_cflag |= PARODD;
      s2eLogMessage
	  ("s2ePortSetup : Activeaza verificarea paritatii la TX si RX pt valoare impara pentru conexiunea %d\n",
	   s);
      break;
    }
  }
  // masca pentru setarea dimensiunii unui caracter in informatia transmisa
  cfg->c_cflag &= ~CSIZE;
  // se extrage capacitatea de reprezentare a unui caracter 
  switch (data) {
    // seteaza campul corespunzator reprezentarii 
    // unui caracter cu macroul corespunzator
  case 5:
    {
      cfg->c_cflag |= CS5;	// 5 biti / char
      s2eLogMessage
	  ("s2ePortSetup : Seteaza %d biti / char pentru conexiunea %d\n",
	   data, s);
      break;
    }
  case 6:
    {
      cfg->c_cflag |= CS6;	// 6 biti / char
      s2eLogMessage
	  ("s2ePortSetup : Seteaza %d biti / char pentru conexiunea %d\n",
	   data, s);
      break;
    }
  case 7:
    {
      cfg->c_cflag |= CS7;	// 7 biti / char
      s2eLogMessage
	  ("s2ePortSetup : Seteaza %d biti / char pentru conexiunea %d\n",
	   data, s);
      break;
    }
  case 8:
    {
      cfg->c_cflag |= CS8;	// 8 biti / char
      s2eLogMessage
	  ("s2ePortSetup : Seteaza %d biti / char pentru conexiunea %d\n",
	   data, s);
      break;
    }
  }
  // se extrage numerul de biti de stop si ajusteaza valoare de configurare
  if (stopb == 1) {
    cfg->c_cflag &= ~CSTOPB;	// seteaza un bit de STOP
    s2eLogMessage
	("s2ePortSetup : Seteaza %d biti biti de stop pentru conexiunea %d\n",
	 stopb, s);
  } else {
    cfg->c_cflag |= CSTOPB;	// seteaza 2 biti de STOP 
    s2eLogMessage
	("s2ePortSetup : Seteaza 2 biti de stop pentru conexiunea %d\n",
	 s);

  }
  // seteaza valorile de configurare primite la intrare si setate in structura termios
  // pentru portul serial considerat in s
  // setarea are loc imediat si apoi se revine cu un cod de return intreg 

  return tcsetattr(s, TCSANOW, cfg);
}


int main(int argc, char **argv)
{
  struct sockaddr_in si_other;	// structura care implementeaza lucrul cu adrese IP
  int s, i;			// variabile auxiliare
  socklen_t slen = sizeof(si_other);	// dimensiunea adresei IP
  char buf[BUFLEN];		// buffer date RX/TX
  int ser_dev;			// identificator dispozitiv serial      
  struct termios cfg;		// structura cu date de configurare a portului serial
  char *bufptr;			// caracterul curent in buffer
  int nbytes;			// numarul de octeti cititi
  int cnt;			// contor
  int ret;			// cod de return
  int n_pack;			// numarul de pachete transmise
  unsigned char parity;		// paritate
  int speed, data, stopb;	// baudrate, byte/char si numar biti de stop

  if (argc != 7) {		// daca numarul de parametri nu este corespunzator 
    // afiseaza help
    s2eCommandLine();
    return 1;
  }

  s2eLogMessage
      ("s2eConverter : ===================================================%s",
       "\n");
  s2eLogMessage
      ("s2eConverter : Convertorul Serial la Ethernet a fost lansat !%s",
       "\n");

  // se creeaza un socket pentru trimiterea informatiei sub forma de datagrame   
  if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    s2eLogError("socket");
  // aloca memorie pentru membrii structurii IP si seteaza valori de initializare
  memset((char *) &si_other, 0, sizeof(si_other));
  // seteaza familia de socket
  si_other.sin_family = AF_INET;
  // se preia portul de comuicatie in format specific
  si_other.sin_port = htons(atoi(argv[3]));
  // conversia adresei IP din format generic string in struct addr_in
  if (inet_aton(argv[2], &si_other.sin_addr) == 0) {
    s2eLogMessage("s2eConverter : Conversia a esuat pentru %s\n", argv[2]);
    exit(EXIT_FAILURE);
  }
  // deschide portul serial
  if ((ser_dev = s2eOpenSerialPort(argv[5])) == -1)
    s2eLogMessage("s2eConverter : Portul serial %d nu poate fi deschis!\n",
		  ser_dev);
  // parametrizeaza conexiunea seriala
  sscanf(argv[6], "%d-%d%c%d", &speed, &data, &parity, &stopb);
  ret = s2ePortSetup(ser_dev, &cfg, speed, data, parity, stopb);
  if (ret < 0) {
    s2eLogMessage("s2eConverter : Portul serial %s nu se poate configura!",
		  argv[5]);
    exit(EXIT_FAILURE);
  }
  // se testeaza daca se doreste afisarea mesajelor de stare in timpul executiei
  if ((0 == strcmp(argv[8], "--verbose"))
      || (0 == strcmp(argv[8], "-v")))
    verbose = 1;

  while (1) {

    s2eLogMessage("s2eConverter : Se transmite pachetul %d\n", i);

    // se initializeaza pointerul in buffer la primul element   
    bufptr = buf;
    // se citesc octeti de pe linia seriala
    while ((nbytes =
	    read(ser_dev, bufptr, buf + sizeof(buf) - bufptr - 1)) > 0) {
      bufptr += nbytes;
      cnt++;
      // dimensionam dimensiunea datelor pentru afisare 
      if (cnt == BUFLEN / 4)
	break;
    }

    printf("%s\n", buf);

    // se impacheteaza si se transmite informatia de pe seriala pe linkul UDP/IP
    if (sendto(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, slen) ==
	-1)
      s2eLogError("sendto()");
  }
  // inchide conexiunea seriala
  close(s);

  s2eLogMessage
      ("s2eConverter : Aplicatia a revenit dupa transmiterea a %d pachete !\n",
       n_pack);
  s2eLogMessage
      ("s2eConverter : ===================================================%s",
       "\n");

  return 0;
}
