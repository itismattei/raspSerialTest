/*
 * questo programma permette di testare la classe SerialComm.
 * Essa permette di usare la comunicazione seriale UART della
 * rapsberry. La modalita' di test è in loopback.
 * 
 */


#include <iostream>
#include <fstream>
using namespace std;

#include "SerialComm.h"
#include "TRcmd.h"
#include <stdio.h>
#include <stdlib.h>
#include <wiringSerial.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

static uint64_t epochMilli, epochMicro ;


/*
 * delay:
 *      Wait for some number of milliseconds
 *********************************************************************************
 */

void delay (unsigned int howLong)
{
  struct timespec sleeper, dummy ;

  sleeper.tv_sec  = (time_t)(howLong / 1000) ;
  sleeper.tv_nsec = (long)(howLong % 1000) * 1000000 ;

  nanosleep (&sleeper, &dummy) ;
}



/*
 * millis:
 *      Return a number of milliseconds as an unsigned int.
 *********************************************************************************
 */

unsigned int millis (void)
{
  struct timeval tv ;
  uint64_t now ;

  gettimeofday (&tv, NULL) ;
  now  = (uint64_t)tv.tv_sec * (uint64_t)1000 + (uint64_t)(tv.tv_usec / 1000) ;

  return (uint32_t)(now - epochMilli) ;
}
/// programma principale
///

int main(){
	unsigned int nextTime ;

	char buff[8] = "salve\0", buff1[8] = "\n";
	char ricBuff[256];
	SerialComm uart;
	TxRxcmd PIPE;
//	uart.openSer();
//	printf("Num descrittore %d\n", uart.mfd);

	/// collego PIPE ad uart
	PIPE.connect(&uart);
	
	int contatore = 0;
	while(1){
		++contatore;
		int j;
		/// stampa i bytes che inviera'
		printf("%d invio il comando 'D' 1 ", contatore);

		/* 1 */
		PIPE.sendCmd('D', 1);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());

		/* 2 */
		printf("%d invio il comando 'D' 2 ", contatore);
		PIPE.sendCmd('D', 2);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());

		/* 3 */
		printf("%d invio il comando 'D' 3 ", contatore);
		PIPE.sendCmd('D', 3);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());
		
		/* 4 */
		printf("%d invio il comando 'D' 4 ", contatore);
		PIPE.sendCmd('D', 4);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());
		
		/* 5 */
		printf("%d invio il comando 'D' 5 ", contatore);
		PIPE.sendCmd('D', 5);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());
		
		/* 6 giroscopio */
		printf("----\n%d invio il comando 'D' 6 ", contatore);
		PIPE.sendCmd('D', 6);
		/// attende 5 ms
		nextTime = millis () + 10 ;
		while(millis() < nextTime);
		/// legge il buffer di ricezione e memorizza i bytes disponibili
		j = PIPE.receiveCmd();
		if (j)
			printf("%d\n", PIPE.convertiDatoRaw());
		
		printf("*** ***\n");
		sleep(1);
	}

	return 0;
}